#include <iostream>
#include <vector>
#include "lodepng.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Image {
    int width;
    int height;
    std::vector<uint8_t> data;
};

Image readPNG(const std::string& filename) {
    std::vector<uint8_t> image;
    unsigned width, height;
    unsigned error = lodepng::decode(image, width, height, filename);
    if (error) {
        std::cout << "Error while decoding: " << lodepng_error_text(error) << std::endl;
        return Image();
    }

    Image result;
    result.width = width;
    result.height = height;
    result.data = std::move(image);

    return result;
}

void savePNG(const std::string& filename, const Image& image) {
    unsigned error = lodepng::encode(filename, image.data, image.width, image.height);
    if (error) {
        std::cout << "Error while encoding: " << lodepng_error_text(error) << std::endl;
    }
}

Image rotateImage(const Image& inputImage, float angle) {
    float radAngle = angle * M_PI / 180.0f;

    Image rotatedImage;
    rotatedImage.width = inputImage.width;
    rotatedImage.height = inputImage.height;
    rotatedImage.data.resize(inputImage.data.size());

    int cx = inputImage.width / 2;
    int cy = inputImage.height / 2;

    for (int y = 0; y < inputImage.height; ++y) {
        for (int x = 0; x < inputImage.width; ++x) {
            float dx = static_cast<float>(x - cx);
            float dy = static_cast<float>(y - cy);

            int newX = static_cast<int>(std::round(dx * std::cos(radAngle) - dy * std::sin(radAngle))) + cx;
            int newY = static_cast<int>(std::round(dx * std::sin(radAngle) + dy * std::cos(radAngle))) + cy;

            if (newX >= 0 && newX < inputImage.width && newY >= 0 && newY < inputImage.height) {
                for (int k = 0; k < 4; ++k) {
                    rotatedImage.data[(y * inputImage.width + x) * 4 + k] =
                        inputImage.data[(newY * inputImage.width + newX) * 4 + k];
                }
            }
        }
    }

    return rotatedImage;
}

Image flipImageHorizontal(const Image& inputImage) {
    Image flippedImage = inputImage;
    for (int i = 0; i < inputImage.height; ++i) {
        for (int j = 0; j < inputImage.width / 2; ++j) {
            for (int k = 0; k < 4; ++k) {
                std::swap(flippedImage.data[i * inputImage.width * 4 + j * 4 + k],
                    flippedImage.data[i * inputImage.width * 4 + (inputImage.width - j - 1) * 4 + k]);
            }
        }
    }
    return flippedImage;
}

Image flipImageVertical(const Image& inputImage) {
    Image flippedImage = inputImage;
    for (int i = 0; i < inputImage.height / 2; ++i) {
        for (int j = 0; j < inputImage.width; ++j) {
            for (int k = 0; k < 4; ++k) {
                std::swap(flippedImage.data[i * inputImage.width * 4 + j * 4 + k],
                    flippedImage.data[(inputImage.height - i - 1) * inputImage.width * 4 + j * 4 + k]);
            }
        }
    }
    return flippedImage;
}

Image applyGrayscaleFilter(const Image& inputImage) {
    Image filteredImage = inputImage;
    for (size_t i = 0; i < inputImage.data.size(); i += 4) {
        uint8_t pixel = (inputImage.data[i] + inputImage.data[i + 1] + inputImage.data[i + 2]) / 3;
        filteredImage.data[i] = filteredImage.data[i + 1] = filteredImage.data[i + 2] = pixel;
    }
    return filteredImage;
}

int main() {
    std::string inputFilePath;
    std::cout << "** ONLY .png files are supported ** \n";
    std::cout << "Enter the file path: ";
    std::cin >> inputFilePath;

    Image inputImage = readPNG(inputFilePath);
    if (inputImage.data.empty()) {
        return 1;
    }

    int action;
    std::cout << "\n";
    std::cout << "What do you want to do with the image?\n";
    std::cout << "1. Rotate\n";
    std::cout << "2. Flip Vertically\n";
    std::cout << "3. Flip Horizontally\n";
    std::cout << "4. Grayscale\n";
    std::cout << "Enter the corresponding number: ";
    std::cin >> action;

    Image modifiedImage;
    switch (action) {
    case 1:
        float rotationAngle;
        std::cout << "Enter the rotation angle: ";
        std::cin >> rotationAngle;
        modifiedImage = rotateImage(inputImage, rotationAngle);
        break;
    case 2:
        modifiedImage = flipImageVertical(inputImage);
        break;
    case 3:
        modifiedImage = flipImageHorizontal(inputImage);
        break;
    case 4:
        modifiedImage = applyGrayscaleFilter(inputImage);
        break;
    default:
        std::cout << "Invalid choice\n";
        return 1;
    }

    savePNG("modified.png", modifiedImage);
    std::cout << "\n";
    std::cout << "Image saved as 'modified.png'.\n";
    std::cout << "\n";

    std::cout << "\n";
    std::cout << "Press Enter to exit the program." << std::endl;
    std::cin.ignore();
    std::cin.get();

    return 0;
}