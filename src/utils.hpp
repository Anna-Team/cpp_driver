//
// Created by felix on 08.02.23.
//

#ifndef ANNADB_DRIVER_UTILS_HPP
#define ANNADB_DRIVER_UTILS_HPP

#include <ranges>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

namespace utils
{
    /**
     * Helper to split up a string by a delimiter and put them into a std::vector
     * with the range library
     *
     * @param txt original string
     * @param delim the char on which to split
     * @return the strings inside of a vector
     */
    inline std::vector<std::string> split(std::string_view txt, char delim)
    {   const auto to_string = [](auto && chr) -> std::string {
            const std::ranges::common_view cv = std::ranges::common_view{chr};
            return {cv.begin(), cv.end()};
        };

        const auto range_ = std::ranges::split_view(txt, delim) | std::ranges::views::transform(to_string);

        std::vector<std::string> vec;
        vec.reserve(std::distance(range_.begin(), range_.end()));

        std::ranges::copy(range_, std::back_inserter(vec));

        return vec;
    }
}
#endif //ANNADB_DRIVER_UTILS_HPP
