#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>


#define SUBCHUNK1SIZE   (16)
#define AUDIO_FORMAT    (1) /*For PCM*/
#define NUM_CHANNELS    (2)
#define SAMPLE_RATE     (44100)

#define BITS_PER_SAMPLE (16)

#define BYTE_RATE       (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE/8)
#define BLOCK_ALIGN     (NUM_CHANNELS * BITS_PER_SAMPLE/8)


typedef struct wavfileHeader
{
	char    ChunkID[4];     /*  4   */
	int32_t ChunkSize;      /*  4   */
	char    Format[4];      /*  4   */

	char    Subchunk1ID[4]; /*  4   */
	int32_t Subchunk1Size;  /*  4   */
	int16_t AudioFormat;    /*  2   */
	int16_t NumChannels;    /*  2   */
	int32_t SampleRate;     /*  4   */
	int32_t ByteRate;       /*  4   */
	int16_t BlockAlign;     /*  2   */
	int16_t BitsPerSample;  /*  2   */

	char    Subchunk2ID[4];
	int32_t Subchunk2Size;
} s_wavfileHeader;

/*Data structure to hold a single frame with two channels*/

typedef struct stereo16PCM
{
	int16_t left;
	int16_t right;
} s_stereo16PCM;

typedef s_wavfileHeader* s_Header;


int Create16stereo(FILE* file_p,int32_t SampleRate,int32_t FrameCount)
{
	int ret;

	s_wavfileHeader wavHeader;
	int32_t subchunk2_size;
	int32_t chunk_size;

	size_t write_count;

	subchunk2_size  = FrameCount * NUM_CHANNELS * BITS_PER_SAMPLE/8;
	chunk_size      = 4 + (8 + SUBCHUNK1SIZE) + (8 + subchunk2_size);

	wavHeader.ChunkID[0] = 'R';
	wavHeader.ChunkID[1] = 'I';
	wavHeader.ChunkID[2] = 'F';
	wavHeader.ChunkID[3] = 'F';

	wavHeader.ChunkSize = chunk_size;

	wavHeader.Format[0] = 'W';
	wavHeader.Format[1] = 'A';
	wavHeader.Format[2] = 'V';
	wavHeader.Format[3] = 'E';

	wavHeader.Subchunk1ID[0] = 'f';
	wavHeader.Subchunk1ID[1] = 'm';
	wavHeader.Subchunk1ID[2] = 't';
	wavHeader.Subchunk1ID[3] = ' ';

	wavHeader.Subchunk1Size = SUBCHUNK1SIZE;
	wavHeader.AudioFormat = AUDIO_FORMAT;
	wavHeader.NumChannels = NUM_CHANNELS;
	wavHeader.SampleRate = SampleRate;
	wavHeader.ByteRate = BYTE_RATE;
	wavHeader.BlockAlign = BLOCK_ALIGN;
	wavHeader.BitsPerSample = BITS_PER_SAMPLE;

	wavHeader.Subchunk2ID[0] = 'd';
	wavHeader.Subchunk2ID[1] = 'a';
	wavHeader.Subchunk2ID[2] = 't';
	wavHeader.Subchunk2ID[3] = 'a';
	wavHeader.Subchunk2Size = subchunk2_size;

	write_count = fwrite(&wavHeader,sizeof(s_wavfileHeader), 1,file_p);

	ret = (1 != write_count)? -1 : 0;

	return ret;
}

s_stereo16PCM *stereoBufferallocate(int32_t FrameCount)
{
	return (s_stereo16PCM *)malloc(sizeof(s_stereo16PCM) * FrameCount);
}

size_t  write_PCM16wav_data(FILE* file_p,int32_t FrameCount,s_stereo16PCM  *buffer_p)
{
	size_t ret;
	ret = fwrite(buffer_p,sizeof(s_stereo16PCM), FrameCount,file_p);
	return ret;
}

int wavTypeConvert(char *path)
{
	int ret;

	FILE* file_p;

	int count=0;
	
	size_t written;

	s_stereo16PCM  *buffer_p = NULL;

	double duration,sample_count;
	
	FILE * infile = fopen(path,"rb");

	if(NULL == infile)
	{
		perror("fopen fail in infile");
		ret = -1;
	}


	s_Header meta = (s_Header)malloc(sizeof(s_wavfileHeader));

	fread(meta, 1, sizeof(s_wavfileHeader), infile);

	short int *dataP = malloc(meta->Subchunk2Size);

	while(fread(dataP,1,meta->Subchunk2Size,infile) > 0)

	sample_count = (meta->Subchunk2Size)/2;  	

	duration = sample_count/SAMPLE_RATE;
	
	int32_t FrameCount = duration * SAMPLE_RATE;

	/*Open the wav file*/
	file_p = fopen(path, "w");
	if(NULL == file_p)
	{
		perror("fopen failed in main");
		ret = -1;
	}

	/*Allocate the data buffer*/
	buffer_p = stereoBufferallocate(FrameCount);
	if(NULL == buffer_p)
	{
		perror("fopen failed in main");
		ret = -1;
	}

	/*Write the wav file header*/
	ret = Create16stereo(file_p,SAMPLE_RATE,FrameCount);
	if(ret < 0)
	{
		perror("write_PCM16_stereo_header failed in main");
		ret = -1;
	}

	for(count=0;count <= FrameCount;count++)
	{
		buffer_p[count].left  = dataP[count];
		buffer_p[count].right = dataP[count];
	}

	/*Write the data out to file*/
	written = write_PCM16wav_data(  file_p,
			FrameCount,
			buffer_p);
	if(written < FrameCount)
	{
		perror("write_PCM16wav_data failed in main");
		ret = -1;
	}

	/*Free and close everything*/
	free(buffer_p);
	free(meta);
	fclose(file_p);
	fclose(infile);
	return ret;
}
