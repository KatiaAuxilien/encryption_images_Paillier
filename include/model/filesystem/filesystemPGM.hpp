/**
 * \file filesystementPGM.hpp
 * \brief Class containing functions that allow to retrieve images in .pgm format.
 * \author Katia Auxilien
 * \date April 2024
 * \details Class inspired by Functions.h of Norman Hutte.
 */
#ifndef FILESYSTEM_PGM_LIBRARY
#define FILESYSTEM_PGM_LIBRARY
#include <iostream>
#include <vector>
#include <random>
#include <bitset>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include <string>

/**
 * \class filesystemPGM
 * \brief Utility class for handling .pgm image files.
 * \details This class provides functions for checking if a file extension is .pgm and for retrieving the file paths of all .pgm images in a folder. It is inspired by Norman Hutte's Functions.h.
 * \author Katia Auxilien
 */
class filesystemPGM
{

public:
    /**
     *  \brief Check if the file extension is .pgm
     *  \param extension The file extension to check
     *  \return True if the extension is .pgm, false otherwise
     *  \authors Norman Hutte, Katia Auxilien
     *  \date 11 April 2024
     */
    static bool is_pgm(const std::string &extension);

    /**
     * \brief Get the file paths of all .pgm images in a folder
     * \param imagePaths Vector to store the file paths of the .pgm images
     * \param folderPath Path of the folder to search for .pgm images
     * \details This function clears the imagePaths vector and then iterates through all the files in the folder specified by folderPath. If a file is a regular file (not a directory) and its extension is .pgm, the file path is added to the imagePaths vector.
     * \authors Norman Hutte, Katia Auxilien
     * \date 11 April 2024
     */
    static void getFilePathsOfPGMFilesFromFolder(std::vector<std::string> &imagePaths, std::string folderPath);
};

#endif // FILESYSTEM_PGM_LIBRARY