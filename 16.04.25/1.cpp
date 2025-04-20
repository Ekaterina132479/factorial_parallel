#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>


std::mutex frame_mutex;
std::mutex result_mutex;
std::queue<cv::Mat> frame_queue;
std::queue<cv::Mat> result_queue;
std::atomic<bool> stop_processing(false);
std::condition_variable frame_cv; 

const size_t MAX_QUEUE_SIZE = 1; 

void process_frames(cv::CascadeClassifier& face_cascade,
    cv::CascadeClassifier& eyes_cascade,
    cv::CascadeClassifier& smile_cascade) {
    while (!stop_processing) {
        cv::Mat frame;


        std::unique_lock<std::mutex> lock(frame_mutex);
        frame_cv.wait(lock, []() { return !frame_queue.empty() || stop_processing; });

        if (frame_queue.empty()) {
            lock.unlock();
            continue;
        }

        frame = frame_queue.front();
        frame_queue.pop();
        lock.unlock();

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);

        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0, cv::Size(80, 80));

        for (const auto& face : faces) {
            cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);

            cv::Mat faceROI = gray(face);

            std::vector<cv::Rect> eyes;
            eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 10, 0, cv::Size(40, 40));
            for (const auto& eye : eyes) {
                cv::Point center(face.x + eye.x + eye.width / 2, face.y + eye.y + eye.height / 2);
                cv::circle(frame, center, eye.width / 2, cv::Scalar(0, 255, 0), 2);
            }

            std::vector<cv::Rect> smiles;
            smile_cascade.detectMultiScale(faceROI, smiles, 1.5, 15, 0, cv::Size(30, 30));
            for (const auto& smile : smiles) {
                cv::Rect smileRect(face.x + smile.x, face.y + smile.y, smile.width, smile.height);
                cv::rectangle(frame, smileRect, cv::Scalar(0, 255, 255), 2);
            }
        }


        std::lock_guard<std::mutex> result_lock(result_mutex);
        result_queue.push(frame.clone());
    }
}

int main() {

    cv::CascadeClassifier face_cascade, eyes_cascade, smile_cascade;
    if (!face_cascade.load("C:/opencv/build/etc/haarcascades/haarcascade_frontalface_default.xml") ||
        !eyes_cascade.load("C:/opencv/build/etc/haarcascades/haarcascade_eye.xml") ||
        !smile_cascade.load("C:/opencv/build/etc/haarcascades/haarcascade_smile.xml")) {
        std::cerr << "Ошибка: не удалось загрузить один из классификаторов!" << std::endl;
        return -1;
    }


    cv::VideoCapture cap("ZUA.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть видео!" << std::endl;
        return -1;
    }


    const int num_threads = 4; 
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(process_frames,
            std::ref(face_cascade),
            std::ref(eyes_cascade),
            std::ref(smile_cascade));
    }

    cv::Mat frame;
    while (cap.read(frame)) {
        std::lock_guard<std::mutex> lock(frame_mutex);

  
        if (frame_queue.size() < MAX_QUEUE_SIZE) {
            frame_queue.push(frame.clone());
            frame_cv.notify_one(); 
        }

  
        std::lock_guard<std::mutex> result_lock(result_mutex);
        if (!result_queue.empty()) {
            cv::imshow("Распознавание лица, глаз и улыбки", result_queue.front());
            result_queue.pop();
        }

      
        if (cv::waitKey(30) == 27) {
            stop_processing = true;
            break;
        }
    }

 
    stop_processing = true;
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
