/*
*   Author: Ibrahim Sardar
*   03/09/2018
*/

#include "pnmio.h"  /* REQUIRES C++11 */
#include "llip.h"   /* LOW LEVEL PROCESSING FUNCTIONS HERE */

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include "math.h"

// helpers
void say(std::string s) {std::cout<<s<<std::endl;}
void save(Image img, std::string original, pnmio image_manager) {
    // ask for new filename for processed image
    say("Enter a name for the processed version of " + original);
    std::string newfilename;
    std::getline(std::cin, newfilename);
    try {
        image_manager.store_image(newfilename, img);
        say("\""+newfilename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+newfilename+"\" failed to save.");
    }
}
void quicksave(Image img, std::string filename, std::string addon, pnmio image_manager) {
    // insert add-on before ".xxx" and save
    int dot_index = (int) filename.find_last_of(".");
    try {
        image_manager.store_image(filename.insert(dot_index, addon), img);
        say("\""+filename+"\" saved successfully.");
    } catch (std::exception &e) {
        say("\""+filename+"\" failed to save.");
    }
}

int main() {

    // init...
    say("Initializing...");

    // print instructions
    say("Welcome to the Low Level Image Processor."
        "This program offers 5 different image processing algorithms to choose from."
        "Type 'q' or 'quit' to quit.");

    // menu
    while (true) {

        // ask for filename to test
        say("\nEnter a filename (in this exe's directory) of an image to process:");
        std::string filename;
        std::getline(std::cin, filename);

        // check quit flag
        if (filename == "quit" || filename == "QUIT")
            break;

        // read image
        pnmio manager;
        Image input;
        try {input = manager.load_image(filename);}
        catch (std::exception &e) {say(e.what()); break;};

        // notify the user the image is being converted to gray-scale if it is RGB; warn/notify otherwise
        if (input.type == 1) {
            manager.convert_rgb_to_gray(input);
            say("RGB image detected; converted to gray-scale for compatibility.");
        } else if (input.type == 0) {
            say("Gray-scale image detected.");
        } else {
            say("Unknown image-type detected; some features may behave unexpectedly.");
        }

        // ask for processing type
        say("\nEnter a processing type: (type the index)\n"
            "0: Quick Test All\n"
            "1: Histogram Normalization\n"
            "2: Logarithmic Mapping\n"
            "3: Centered Rotation\n"
            "4: Gaussian Averaging Filter\n"
            "5: Median Filter (3x3 neighborhood)");
        std::string ptype;
        std::getline(std::cin, ptype);
        say("");

        // check quit flag
        if (filename == "quit" || filename == "QUIT")
            break;

        // process image and save it as a new image file
        switch (std::stoi(ptype)) {
        case 0: {
            // save original starting input data vector
            std::vector<unsigned int> original = input.data;

            // 1
            llip::histogram_equalization(input);
            quicksave(input, filename, "_1_hist", manager);
            say("Histogram Normalization Done.");
            input.data = original;

            // 2
            llip::logarithmic_mapping(input, 1);
            quicksave(input, filename, "_2_log", manager);
            say("Logarithmic Mapping Done.");
            input.data = original;

            // 3
            llip::centered_rotation(input, 45);
            quicksave(input, filename, "_3_rot45", manager);
            say("Centered Image Rotation Done. (angle=45)");
            input.data = original;

            // 4
            llip::gaussian_averaging_filter(input, 1);
            quicksave(input, filename, "_4_gauss", manager);
            say("Gaussian Average Filtering Done (Blurs). (sigma=1)");
            input.data = original;

            // 5
            llip::median_neighborhood_filter(input);
            quicksave(input, filename, "_5_median", manager);
            say("Median Smooth Filtering Done (Smooths).");
            break;

        } case 1:
            llip::histogram_equalization(input);
            save(input, filename, manager);
            break;

        case 2:
            llip::logarithmic_mapping(input, 1);
            save(input, filename, manager);
            break;

        case 3: {
            say("Enter a rotation angle (degrees):");
            std::string angle;
            std::getline(std::cin, angle);
            llip::centered_rotation(input, std::stod(angle));
            save(input, filename, manager);
            break;

        } case 4: {
            say("Enter a value for sigma: (recommended: 1)");
            std::string sigma;
            std::getline(std::cin, sigma);
            llip::gaussian_averaging_filter(input, std::stod(sigma));
            save(input, filename, manager);
            break;

        } case 5:
            llip::median_neighborhood_filter(input);
            save(input, filename, manager);
            break;

        default:
            say("Input could not be understood.");
            break;
        }
    }

    // end program
    return 0;
}
