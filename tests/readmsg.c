/*
 *  Copyright (c) 2008-2009, Jeremy Sandell <jlsandell@gmail.com>
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 *  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <hl7c/proto.h>

bool
testread(int argc, char **argv)
{
    const char *msg = "MSH|^~\\&|SendingApp|SendingFacility|ReceivingApplication|ReceivingFacility|20090811203018||ADT^A04||P|2.3|||||||\rEVN|A04|20090811203018||||\rPID|1|13885|13885||Public^John^Q^^^||19700101|M|||1234 Test Avenue^Pat Line 2^Nashville^TN^12345^US^^^^^||(555)555-1234^PRN^PH|(555)555-1234^WPN^PH|M|||13885|111-22-3344|||||||||||\rPV1|1|O||R|||DOC^Doctor^Test^M.D.^^^|DOC^Doctor^Test^MD^^^||||||||||||||||||||||||||||||||||||||||||||\rGT1|1|13886|Public^Jane^Q^^^||1234 Test Ln.^Gua Line 2^Nashville^TN^12345^US^^^^^|(555)555-1234^PRN^PH|(800)555-1234^WPN^PH|||||111-22-4433|||||^^^^^US^^^^^|||1- Employed full time|||||||||||||||||||||||||||||||||||\rIN1|1|UHC3||United Health Care|P.O. Box 12345^^Atlanta^GA^12345^US^^^^^||(555)555-5555^WPN^PH|IN1 GROUP NAME||||20081201|20151201|||Public^John^Q^^^|1- Self|19700101|1234 Test Avenue^^Nashville^TN^12345^US^^^^^|Y|||||||Y|||||||||111223344|0.00|||||1- Employed full time|M|^^^^^US^^^^^|||||111223344\rIN1|2|BCBS||Blue Cross Blue Shield|P.O. Box 12345^^Salt Lake City^UT^12345^US^^^^^||(555)555-5555^WPN^PH|IN2 GROUP NAME||||20081201|20151201|||Public^Jane^Q^^^|2- Spouse|19040101|1234 Test Avenue^^Nashville^TN^12345^US^^^^^|Y|||||||Y|||||||||111223344|0.00|||||1- Employed full time|F|^^^^^US^^^^^|||||111223344\r\r";
    bool ret;
    ret = readmsg(msg);
    return ret;
}
