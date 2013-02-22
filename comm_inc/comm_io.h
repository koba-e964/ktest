#ifndef COMM_INC_IO_H_0d508306
#define COMM_INC_IO_H_0d508306
/**
	Common Constants that are used both by kernel and by applications.
*/
enum OpeningAttrib
{
	KOS_OPEN_READ=1,
	KOS_OPEN_WRITE=2,
	KOS_OPEN_BINARY=4,
	KOS_OPEN_SEQUENTIAL=8,
	KOS_OPEN_EXISTING=16,
	KOS_OPEN_CREATE=32,
	KOS_OPEN_TRUNCATE=64,
};
enum StdStream
{
	STDIN=0,
	STDOUT=1,
	STDERR=2,
};

#endif

