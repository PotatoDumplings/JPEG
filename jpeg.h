typedef struct _SOI
{
  uint8_t SOI[2];
} SOI;

typedef struct _JFIFHeader
{
  uint8_t APP0[2];         /* 02h  Application Use Marker    */
  uint8_t Length[2];       /* 04h  Length of APP0 Field      */
  uint8_t Identifier[5];   /* 06h  "JFIF" (zero terminated) Id String */
  uint8_t Version[2];      /* 07h  JFIF Format Revision      */
  uint8_t Units;           /* 09h  Units used for Resolution */
  uint8_t Xdensity[2];     /* 0Ah  Horizontal Resolution     */
  uint8_t Ydensity[2];     /* 0Ch  Vertical Resolution       */
  uint8_t XThumbnail;      /* 0Eh  Horizontal Pixel Count    */
  uint8_t YThumbnail;      /* 0Fh  Vertical Pixel Count      */
} JFIFHEAD;

/*
SOI = 0xFF, 0xD8
APP0 = 0xFF, 0xE0
Length = 16
Identifier = "JFIF\null"
Version = 0x01, 0x02
units = 0x0
xdensity = x res
ydensity = y res
xthumb = 0
ythumb = 0
*/

typedef struct _DQT
{
  uint8_t marker;
  uint8_t type;
  uint8_t length[2];
  uint8_t preid;
  uint8_t table[64];
} DQT;

/*
marker = 0xff
type = 0xdb
length = 69
preid = upper 4: 8, lower four = table id
table = quantisation table
*/

typedef struct _HUFFMANHead
{
  uint8_t marker;
  uint8_t type;
  uint8_t length[2];
  //uint8_t classid;
  //uint8_t codelengths[16];
} HUFFMANHead;

typedef struct _HUFFMANTable
{
  uint8_t classid;
  uint8_t codelengths[16];
} HuffT;

/*
marker = 0xff
type = 0xc4
length = 21 + num of codes
classid = upper 4: 0 if DC, 1 if AC; lower four table id
codelengths = number of codes of length N

follow with array of bytes to be encoded
*/

typedef struct _STARTofFrame
{
  uint8_t marker;
  uint8_t type;
  uint8_t length[2];
  uint8_t precision;
  uint8_t height[2];
  uint8_t width[2];
  uint8_t components;
} SOF;

/*
marker = 0xff
type = 0xc0
length = 8 + 3 * components
precision = 8
height = y res
width = x res
components = 1 greyscale or 3 color
*/

typedef struct _FRAMEComponent
{
  uint8_t id;
  uint8_t sampling;
  uint8_t quant_table;
} FComp;

/*
id = 1 Y, 2 Cb, 3 Cr
sampling = sampling factors = 0x44
quant_table = number of quant table
*/

typedef struct _STARTOfScan
{
  uint8_t marker;
  uint8_t type;
  uint8_t length[2];
  uint8_t components;
} SOS;

/*
marker = 0xff
type = 0xda
length = 6 + 2 * components
components = 1 greyscale, 3 color

must be followed by SComp structs
*/

typedef struct _SCANComponent
{
  uint8_t id;
  uint8_t huff_table;
} SComp;

/*
id = 1 Y, 2 Cb, 3 Cr
huff_table = upper 4 DC table, lower 4 AC table
*/

typedef struct _ENDOfImage
{
  uint8_t marker;
  uint8_t id;
} EOI;

/*
marker = 0xff
id = 0xd9
*/

void bitwriter(int* counter, uint8_t* buffer, bool bit, FILE* fp) {
  if (bit) {
    *buffer = *buffer + (1 << *counter);
  }
  if (*counter == 0) {
    fwrite(buffer, sizeof(uint8_t), 1, fp);
    if (*buffer == 0xff) {
      *buffer = 0;
      fwrite(buffer, sizeof(uint8_t), 1, fp);
    }
    *buffer = 0;
    *counter = 8;
  }
  *counter = *counter - 1;
}

void flushbuffer(int* counter, uint8_t* buffer, FILE* fp) {
  if (*counter != 7) {
    fwrite(buffer, sizeof(uint8_t), 1, fp);
  }
  *counter = 7;
  *buffer = 0;
}

void finishfile(int* counter, uint8_t* buffer, FILE* fp) {
  EOI eoi;
  eoi.marker = 0xff;
  eoi.id = 0xd9;

  if (*counter != 7) {
    fwrite(buffer, sizeof(uint8_t), 1, fp);
  }

  fwrite(&eoi, sizeof(EOI), 1, fp);

  fclose(fp);
}