/* Copyright (c) 2011 Cody Miller, Daniel Norris, Brett Hitchcock.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef _PIRATES_NULLLOGGER_H_
#define _PIRATES_NULLLOGGER_H_

/**
 * CEGUI logging works by inheriting from CEGUI::Logger and reimplementing 
 * the functions there. This logger effectively "turns off" logging for the 
 * CEGUI system.
 *
 */


class NullLogger : public CEGUI::Logger {
public:
  
  NullLogger() 
  { }
  
  ~NullLogger() 
  { }
  
  void setLoggingLevel(CEGUI::LoggingLevel l) 
  { }
  
  CEGUI::LoggingLevel getLoggingLevel() const 
  { return CEGUI::Informative; }
  
  void logEvent(const CEGUI::String &message, CEGUI::LoggingLevel level) 
  { }
  
  void setLogFilename(const CEGUI::String &filename, bool append) 
  { }
  
private:
  
};


#endif