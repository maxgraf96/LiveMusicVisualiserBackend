/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== COPYING ==================
static const unsigned char temp_binary_data_0[] =
"Copyright (c) 2003-2010 Mark Borgerding\n"
"\n"
"All rights reserved.\n"
"\n"
"Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:\n"
"\n"
"    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.\n"
"    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n"
"    * Neither the author nor the names of any contributors may be used to endorse or promote products derived from this software without specific prior written permission.\n"
"\n"
"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO E"
"VENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR"
" BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH D"
"AMAGE.\n";

const char* COPYING = (const char*) temp_binary_data_0;

//================== README ==================
static const unsigned char temp_binary_data_1[] =
"KISS FFT - A mixed-radix Fast Fourier Transform based up on the principle, \n"
"\"Keep It Simple, Stupid.\"\n"
"\n"
"    There are many great fft libraries already around.  Kiss FFT is not trying\n"
"to be better than any of them.  It only attempts to be a reasonably efficient, \n"
"moderately useful FFT that can use fixed or floating data types and can be \n"
"incorporated into someone's C program in a few minutes with trivial licensing.\n"
"\n"
"USAGE:\n"
"\n"
"    The basic usage for 1-d complex FFT is:\n"
"\n"
"        #include \"kiss_fft.h\"\n"
"\n"
"        kiss_fft_cfg cfg = kiss_fft_alloc( nfft ,is_inverse_fft ,0,0 );\n"
"\n"
"        while ...\n"
"        \n"
"            ... // put kth sample in cx_in[k].r and cx_in[k].i\n"
"            \n"
"            kiss_fft( cfg , cx_in , cx_out );\n"
"            \n"
"            ... // transformed. DC is in cx_out[0].r and cx_out[0].i \n"
"            \n"
"        free(cfg);\n"
"\n"
"    Note: frequency-domain data is stored from dc up to 2pi.\n"
"    so cx_out[0] is the dc bin of the FFT\n"
"    and cx_out[nfft/2] is the Nyquist bin (if exists)\n"
"\n"
"    Declarations are in \"kiss_fft.h\", along with a brief description of the \n"
"functions you'll need to use. \n"
"\n"
"Code definitions for 1d complex FFTs are in kiss_fft.c.\n"
"\n"
"You can do other cool stuff with the extras you'll find in tools/\n"
"\n"
"    * multi-dimensional FFTs \n"
"    * real-optimized FFTs  (returns the positive half-spectrum: (nfft/2+1) complex frequency bins)\n"
"    * fast convolution FIR filtering (not available for fixed point)\n"
"    * spectrum image creation\n"
"\n"
"The core fft and most tools/ code can be compiled to use float, double,\n"
" Q15 short or Q31 samples. The default is float.\n"
"\n"
"\n"
"BACKGROUND:\n"
"\n"
"    I started coding this because I couldn't find a fixed point FFT that didn't \n"
"use assembly code.  I started with floating point numbers so I could get the \n"
"theory straight before working on fixed point issues.  In the end, I had a \n"
"little bit of code that could be recompiled easily to do ffts with short, float\n"
"or double (other types should be easy too).  \n"
"\n"
"    Once I got my FFT working, I was curious about the speed compared to\n"
"a well respected and highly optimized fft library.  I don't want to criticize \n"
"this great library, so let's call it FFT_BRANDX.\n"
"During this process, I learned:\n"
"\n"
"    1. FFT_BRANDX has more than 100K lines of code. The core of kiss_fft is about 500 lines (cpx 1-d).\n"
"    2. It took me an embarrassingly long time to get FFT_BRANDX working.\n"
"    3. A simple program using FFT_BRANDX is 522KB. A similar program using kiss_fft is 18KB (without optimizing for size).\n"
"    4. FFT_BRANDX is roughly twice as fast as KISS FFT in default mode.\n"
"\n"
"    It is wonderful that free, highly optimized libraries like FFT_BRANDX exist.\n"
"But such libraries carry a huge burden of complexity necessary to extract every \n"
"last bit of performance.\n"
"\n"
"    Sometimes simpler is better, even if it's not better.\n"
"\n"
"FREQUENTLY ASKED QUESTIONS:\n"
"\tQ: Can I use kissfft in a project with a ___ license?\n"
"\tA: Yes.  See LICENSE below.\n"
"\n"
"\tQ: Why don't I get the output I expect?\n"
"\tA: The two most common causes of this are \n"
"\t\t1) scaling : is there a constant multiplier between what you got and what you want?\n"
"\t\t2) mixed build environment -- all code must be compiled with same preprocessor \n"
"\t\tdefinitions for FIXED_POINT and kiss_fft_scalar\n"
"\n"
"\tQ: Will you write/debug my code for me?\n"
"\tA: Probably not unless you pay me.  I am happy to answer pointed and topical questions, but \n"
"\tI may refer you to a book, a forum, or some other resource.\n"
"\n"
"\n"
"PERFORMANCE:\n"
"    (on Athlon XP 2100+, with gcc 2.96, float data type)\n"
"\n"
"    Kiss performed 10000 1024-pt cpx ffts in .63 s of cpu time.\n"
"    For comparison, it took md5sum twice as long to process the same amount of data.\n"
"\n"
"    Transforming 5 minutes of CD quality audio takes less than a second (nfft=1024). \n"
"\n"
"DO NOT:\n"
"    ... use Kiss if you need the Fastest Fourier Transform in the World\n"
"    ... ask me to add features that will bloat the code\n"
"\n"
"UNDER THE HOOD:\n"
"\n"
"    Kiss FFT uses a time decimation, mixed-radix, out-of-place FFT. If you give it an input buffer  \n"
"    and output buffer that are the same, a temporary buffer will be created to hold the data.\n"
"\n"
"    No static data is used.  The core routines of kiss_fft are thread-safe (but not all of the tools directory).\n"
"\n"
"    No scaling is done for the floating point version (for speed).  \n"
"    Scaling is done both ways for the fixed-point version (for overflow prevention).\n"
"\n"
"    Optimized butterflies are used for factors 2,3,4, and 5. \n"
"\n"
"    The real (i.e. not complex) optimization code only works for even length ffts.  It does two half-length\n"
"    FFTs in parallel (packed into real&imag), and then combines them via twiddling.  The result is \n"
"    nfft/2+1 complex frequency bins from DC to Nyquist.  If you don't know what this means, search the web.\n"
"\n"
"    The fast convolution filtering uses the overlap-scrap method, slightly \n"
"    modified to put the scrap at the tail.\n"
"\n"
"LICENSE:\n"
"    Revised BSD License, see COPYING for verbiage. \n"
"    Basically, \"free to use&change, give credit where due, no guarantees\"\n"
"    Note this license is compatible with GPL at one end of the spectrum and closed, commercial software at \n"
"    the other end.  See http://www.fsf.org/licensing/licenses\n"
"\n"
"    A commercial license is available which removes the requirement for attribution.  Contact me for details.\n"
"\n"
"  \n"
"TODO:\n"
"    *) Add real optimization for odd length FFTs \n"
"    *) Document/revisit the input/output fft scaling\n"
"    *) Make doc describing the overlap (tail) scrap fast convolution filtering in kiss_fastfir.c\n"
"    *) Test all the ./tools/ code with fixed point (kiss_fastfir.c doesn't work, maybe others)\n"
"\n"
"AUTHOR:\n"
"    Mark Borgerding\n"
"    Mark@Borgerding.net\n";

const char* README = (const char*) temp_binary_data_1;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x63a1442d:  numBytes = 1475; return COPYING;
        case 0x8fd84dae:  numBytes = 5627; return README;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "COPYING",
    "README"
};

const char* originalFilenames[] =
{
    "COPYING",
    "README"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
    {
        if (namedResourceList[i] == resourceNameUTF8)
            return originalFilenames[i];
    }

    return nullptr;
}

}
