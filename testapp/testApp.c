/*Test application for .wav mono file to stereo file*/
#include<stdio.h>

#define PATH "test.wav"

int main(int argc,char **argv)
{
	//wavTypeConvert(PATH);
	wavTypeConvert("/home/tarang/wave/wavConverter/testapp/test.wav");
}
