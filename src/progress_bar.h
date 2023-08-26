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

#ifndef _PROGRESS_BAR_H
#define _PROGRESS_BAR_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdint.h>

/**
 * RAII implementation of a progress bar.
 */
class ProgressBar
{
public:
    /**
     * Constructor.
     * It takes two values: the expected number of iterations whose progress we
     * want to monitor and an initial message to be displayed on top of the bar
     * (which can be updated with updateLastPrintedMessage()).
     */
    ProgressBar(uint32_t expectedIterations, const std::string& initialMessage="");

    /**
     * Destructor to guarantee RAII.
     */
    ~ProgressBar();

    // Make the object non-copyable
    ProgressBar(const ProgressBar& o) = delete;
    ProgressBar& operator=(const ProgressBar& o) = delete;

    /**
     * Must be invoked when the progress bar is no longer needed to restore the
     * position of the cursor to the end of the output.
     * It is automatically invoked when the object is destroyed.
     */
    void endProgressBar();

    /**
     * Prints a new message under the last printed message, without overwriting
     * it. This moves the progress bar down to be placed under the newly
     * written message.
     */
    void printNewMessage(const std::string& message);

    /**
     * Prints a message while the progress bar is on the screen on top on the
     * last printed message. Since the cursor is right at the beginning of the
     * progress bar, it moves the cursor up by one line before printing, and
     * then returns it to its original position.
     */
    void updateLastPrintedMessage(const std::string& message);

    /**
     * Overloaded prefix operator, used to indicate that the has been a new
     * iteration.
     */
    void operator++();

private:
    unsigned int mTotalIterations;
    unsigned int mNumberOfTicks;
    bool mEnded;
    size_t mLengthOfLastPrintedMessage;
};

#endif // _PROGRESS_BAR_H
