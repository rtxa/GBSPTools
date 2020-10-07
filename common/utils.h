#pragma once

#include <string>

namespace GBSPTools {
    constexpr char PathSeparator = '/';

    void ReplaceAll(std::string& source, const std::string& from, const std::string& to) {
        std::string newString;
        newString.reserve(source.length());  // avoids a few memory allocations

        std::string::size_type lastPos = 0;
        std::string::size_type findPos;

        while (std::string::npos != (findPos = source.find(from, lastPos)))
        {
            newString.append(source, lastPos, findPos - lastPos);
            newString += to;
            lastPos = findPos + from.length();
        }

        // Care for the rest after last occurrence
        newString += source.substr(lastPos);

        source.swap(newString);
    }

    void DefaultExtension(std::string& filepath, const std::string ext) {
        // if path doesn't have a .EXT, append extension
        // (extension should include the .)
        for (auto it = filepath.rbegin(); it != filepath.rend(); it++) {
            if (*it != PathSeparator && *it == '.') {
                return;
            }
        }
        filepath = filepath + ext;
    }

    void StripExtension(std::string& filepath) {
        int lastDot = 0;

        for (int i = filepath.size() - 1; i >= 0; i--) {
            if (filepath.at(i) != PathSeparator && filepath.at(i) == '.') {
                lastDot = i;
                break;
            }
        }

        if (lastDot > 0) {
            filepath = filepath.substr(0, lastDot);
        }
    }

    void PathToUnix(std::string& path) {
        GBSPTools::ReplaceAll(path, "\\", "/");
    }
};
