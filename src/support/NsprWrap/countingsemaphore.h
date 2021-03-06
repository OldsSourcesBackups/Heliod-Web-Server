/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright 2008 Sun Microsystems, Inc. All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 *
 * Neither the name of the  nor the names of its contributors may be
 * used to endorse or promote products derived from this software without 
 * specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * implementation of a simple counting semaphore on top of NSPR
 * This replaces COUNTING_SEMAPHORE in ES 3.0
 * Note: THIS CLASS SHOULD NOT BE USED AS A BASE CLASS
 */

#ifndef _NSPR_WRAP_COUNTING_SEMAPHORE_H
#define _NSPR_WRAP_COUNTING_SEMAPHORE_H

#ifdef XP_PC
#ifdef BUILD_NSPRWRAP_DLL
#define COUNTINGSEMAPHORE_DLL_API _declspec(dllexport)
#else
#define COUNTINGSEMAPHORE_DLL_API _declspec(dllimport)
#endif
#else
#define COUNTINGSEMAPHORE_DLL_API
#endif


class COUNTINGSEMAPHORE_DLL_API CountingSemaphore {
        public:
                // Create a semaphore with a max limit
                CountingSemaphore(int initialCount=1);
                ~CountingSemaphore();

		// Blocking acquire of the semaphore
                void acquire();

                // If the semaphore can be acquired, it will do so and return
                // 1. Otherwise 0 is returned indicating failure to acquire
                // the semaphore
                int try_acquire();
                void release();

        private:
                // No copying and assignment allowed
                CountingSemaphore(const CountingSemaphore&);
                CountingSemaphore& operator=(const CountingSemaphore&);
                void* _sem;
};



/*
 * A simple class to automate acquire and release of counting semaphores
 * Use this class with CountingSemaphore objects
 * Usage: 
        void f() 
        {
		CountingSemaphoreLock lock(mysem);
		// code to do some work ...
	} // semaphore is automatically released here
 */


class COUNTINGSEMAPHORE_DLL_API CountingSemaphoreLock {
  public:
    CountingSemaphoreLock(CountingSemaphore& sem) : _semaphore(sem)
        { _semaphore.acquire(); }

    ~CountingSemaphoreLock() { _semaphore.release(); }

  private:
    CountingSemaphoreLock(const CountingSemaphoreLock& copyme);
    CountingSemaphoreLock& operator=(const CountingSemaphoreLock& assignme);
    CountingSemaphore& _semaphore;
};

#endif

