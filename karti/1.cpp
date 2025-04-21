#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <opencv2/opencv.hpp>

namespace fs = std::filesystem;

struct ImageData {
    std::string label;
    cv::Mat img;
    std::vector<cv::KeyPoint> kps;
    cv::Mat desc;
};

std::vector<cv::DMatch> filter_matches(const std::vector<std::vector<cv::DMatch>>& knn_matches, float threshold = 0.5) {
    std::vector<cv::DMatch> selected;
    for (const auto& pair : knn_matches) {
        if (pair[0].distance < threshold * pair[1].distance)
            selected.push_back(pair[0]);
    }
    std::sort(selected.begin(), selected.end(), [](const auto& a, const auto& b) { return a.distance < b.distance; });
    if (selected.size() > 5) selected.resize(5);
    return selected;
}

int main() {
    std::vector<ImageData> templates;

    auto feature_extractor = cv::SIFT::create();
    auto matcher = cv::BFMatcher();

    // Загрузка шаблонов из папки 'cards'
    for (const auto& entry : fs::directory_iterator("./cards")) {
        if (entry.path().extension() == ".png") {
            ImageData data;
            data.label = entry.path().stem().string();
            data.img = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
            if (data.img.empty()) {
                std::cerr << "Ошибка загрузки: " << entry.path() << std::endl;
                continue;
            }
            feature_extractor->detectAndCompute(data.img, cv::noArray(), data.kps, data.desc);
            templates.push_back(data);
            std::cout << "Загружен шаблон: " << data.label << std::endl;
        }
    }

    // Загрузка сцены
    cv::Mat scene = cv::imread("./main_scene.png");
    if (scene.empty()) {
        std::cerr << "Ошибка загрузки сцены!" << std::endl;
        return -1;
    }

    cv::Mat scene_gray;
    cv::cvtColor(scene, scene_gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::KeyPoint> scene_kps;
    cv::Mat scene_desc;
    feature_extractor->detectAndCompute(scene, cv::noArray(), scene_kps, scene_desc);

    // Предобработка изображения (размытие + границы)
    cv::Mat blurred_scene;
    cv::GaussianBlur(scene_gray, blurred_scene, cv::Size(5, 5), 1.5);

    cv::Mat scene_edges;
    cv::Canny(blurred_scene, scene_edges, 60, 180);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(scene_edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat output_img = scene.clone();
    cv::drawContours(output_img, contours, -1, cv::Scalar(255, 100, 0), 1);

    // Поиск совпадений и отрисовка
    for (const auto& template_data : templates) {
        std::vector<std::vector<cv::DMatch>> matches;
        matcher.knnMatch(scene_desc, template_data.desc, matches, 2);
        auto good_matches = filter_matches(matches, 0.5);

        if (!good_matches.empty()) {
            cv::Point2f pos = scene_kps[good_matches[0].queryIdx].pt;

            
            int font = cv::FONT_HERSHEY_DUPLEX;
            double scale = 0.8;
            int thickness = 2;

           
            cv::putText(output_img, template_data.label, pos + cv::Point2f(1, 1), font, scale,
                cv::Scalar(100, 100, 100), thickness, cv::LINE_AA);

           
            cv::putText(output_img, template_data.label, pos, font, scale,
                cv::Scalar(50, 255, 50), thickness, cv::LINE_AA);
        }
    }

    cv::imshow("Результат", output_img);
    cv::waitKey(0);

    return 0;
}