const int PackKeySize = 10;
static unsigned char PackKey[PackKeySize] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x78, 0x2f, 0x3e, 0xe6, 0x7b };

void encode_decode(char* data, int size)
{
	for(int i=0; i<size; i++)
	{
		data[i] = data[i]^PackKey[i%PackKeySize];
	}
}
