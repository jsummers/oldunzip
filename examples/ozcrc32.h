// ozcrc32() is based on public domain code by Jon Mayo, downloaded
// from <http://orangetide.com/code/crc.c>.
// It includes minor changes for Oldunzip. I disclaim any copyright on
// these minor changes. -JS
static uint32_t ozcrc32(const unsigned char *ptr, size_t cnt, uint32_t crc)
{
	static const uint32_t crc32_tab[16] = {
		0x00000000U, 0x1db71064U, 0x3b6e20c8U, 0x26d930acU,
		0x76dc4190U, 0x6b6b51f4U, 0x4db26158U, 0x5005713cU,
		0xedb88320U, 0xf00f9344U, 0xd6d6a3e8U, 0xcb61b38cU,
		0x9b64c2b0U, 0x86d3d2d4U, 0xa00ae278U, 0xbdbdf21cU
	};

	if(cnt==0) return crc;
	crc = ~crc;
	while(cnt--) {
		crc = (crc >> 4) ^ crc32_tab[(crc & 0xf) ^ (*ptr & 0xf)];
		crc = (crc >> 4) ^ crc32_tab[(crc & 0xf) ^ (*ptr++ >> 4)];
	}
	return ~crc;
}
