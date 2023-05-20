/**************************************************************************
 *                                                                        *
 *   Author: Ivo Filot <i.a.w.filot@tue.nl>                               *
 *                                                                        *
 *   BRAMBLE is free software:                                            *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   BRAMBLE is distributed in the hope that it will be useful,           *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#include "progress_bar.h"

#define LENGTH_OF_PROGRESS_BAR 55
#define PERCENTAGE_BIN_SIZE (100.0/LENGTH_OF_PROGRESS_BAR)

namespace
{
    std::string generateProgressBar(unsigned int percentage)
    {
        const int progress = static_cast<int>(percentage/PERCENTAGE_BIN_SIZE);
        std::ostringstream ss;
        ss << " " << std::setw(3) << std::right << percentage << "% ";
        std::string bar("[" + std::string(LENGTH_OF_PROGRESS_BAR-2, ' ') + "]");

        unsigned int numberOfSymbols = std::min(
                std::max(0, progress - 1),
                LENGTH_OF_PROGRESS_BAR - 2);

        bar.replace(1, numberOfSymbols, std::string(numberOfSymbols, '|'));

        ss << bar;
        return ss.str();
    }
}

ProgressBar::ProgressBar(
            uint32_t expectedIterations, const std::string& initialMessage)
    : mTotalIterations(expectedIterations),
      mNumberOfTicks(0),
      mEnded(false)
{
    std::cout << initialMessage << "\n";
    mLengthOfLastPrintedMessage = initialMessage.size();
    std::cout << generateProgressBar(0) << "\r" << std::flush;
}

ProgressBar::~ProgressBar()
{
    endProgressBar();
}

void ProgressBar::operator++()
{
    if (mEnded)
    {
        throw std::runtime_error(
                "Attempted to use progress bar after having terminated it");
    }

    mNumberOfTicks = std::min(mTotalIterations, mNumberOfTicks+1);
    const unsigned int percentage = static_cast<unsigned int>(
            mNumberOfTicks*100.0/mTotalIterations);

    std::cout << generateProgressBar(percentage) << "\r" << std::flush;
}

void ProgressBar::printNewMessage(const std::string& message)
{
    if (mEnded)
    {
        throw std::runtime_error(
                "Attempted to use progress bar after having terminated it");
    }

    std::cout << "\r"
        << std::left
        << std::setw(LENGTH_OF_PROGRESS_BAR + 6)
        << message << "\n";
    mLengthOfLastPrintedMessage = message.size();
    const unsigned int percentage = static_cast<unsigned int>(
            mNumberOfTicks*100.0/mTotalIterations);

    std::cout << generateProgressBar(percentage) << "\r" << std::flush;

}

void ProgressBar::updateLastPrintedMessage(const std::string& message)
{
    if (mEnded)
    {
        throw std::runtime_error(
                "Attempted to use progress bar after having terminated it");
    }

    std::cout << "\r\033[F"
        << std::left
        << std::setw(mLengthOfLastPrintedMessage)
        << message << "\n";
    mLengthOfLastPrintedMessage = message.size();
}

void ProgressBar::endProgressBar()
{
    if (!mEnded)
    {
        std::cout << std::string(2, '\n');
    }
    mEnded = true;
}
