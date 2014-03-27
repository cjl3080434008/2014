#ifndef __BIOSTREAM_H__
#define __BIOSTREAM_H__


class biosexception 
{
public:
	enum BIOSEXCEPTION_CODE {
		none = 0,
		end = 1,		//从流里读数据时读到流的缓冲区末尾
		overflow = 2,	//往流里写数据时超出流的缓冲区末尾
		stringtoolong	//读标准C字符串时超长
	};

	BIOSEXCEPTION_CODE	m_cause;

	biosexception(BIOSEXCEPTION_CODE cause = none):m_cause(cause){}
};

class bios
{
protected:
   char* m_pBegin;
   char* m_pEnd;
   char* m_pCurr;

public:

	bios(){ initialize();}
	virtual ~bios(){};

	void attach(char* buff, long len)
	{
	    m_pBegin = buff;
		m_pCurr = buff;
		m_pEnd = buff+len;
	}

	void detch()
	{
		m_pBegin = 0;
		m_pCurr = 0;
		m_pEnd = 0;
	}

	long avail() const { return (long)(m_pEnd - m_pCurr); }
	long length() const { return (long)(m_pCurr - m_pBegin); }	

private:
	void initialize()
	{
		m_pBegin=m_pCurr=m_pEnd=0;
	}
};

class bostream : public bios
{
public:
	bostream(){};
	~bostream(){};

	inline bostream& operator<<(char);
	inline bostream& operator<<(unsigned char);
	inline bostream& operator<<(signed char);
	inline bostream& operator<<(short);
	inline bostream& operator<<(unsigned short);
	inline bostream& operator<<(int);
	inline bostream& operator<<(unsigned int);
	inline bostream& operator<<(long);
	inline bostream& operator<<(unsigned long);
	inline bostream& operator<<(float);
	inline bostream& operator<<(double);
	inline bostream& operator<<(long double);
	inline bostream& operator<<(bool);
	inline bostream& operator<<(const char*);
	inline bostream& operator<<(char*);
	inline bostream& operator<<(unsigned char*);
	inline bostream& operator<<(const unsigned char*);
	
	bostream& write_buff(const char* buff, long len)
	{
		return write(buff, len); 
	}

private:
	bostream& write(const char* p, long len)
	{
		if(avail() < len)
			throw biosexception(biosexception::overflow);
		switch(len)
		{
		case 1:
			*m_pCurr = *p;
			break;
		case 2:
			*((unsigned short*)((unsigned char*)m_pCurr)) = *((unsigned short*)((unsigned char*)p));
			break;
		case 4:
			*((unsigned long*)((unsigned char*)m_pCurr)) = *((unsigned long*)((unsigned char*)p));
			break;
		default:
			memcpy(m_pCurr,p,len);
			break;
		}
		m_pCurr+=len;
		return *this;
	}
};

class bistream : public bios
{
public:
	bistream(){};
	~bistream(){};

	inline bistream& operator>>(char&);
	inline bistream& operator>>(unsigned char&);
	inline bistream& operator>>(signed char&);
	inline bistream& operator>>(short&);
	inline bistream& operator>>(unsigned short&);
	inline bistream& operator>>(int&);
	inline bistream& operator>>(unsigned int&);
	inline bistream& operator>>(long&);
	inline bistream& operator>>(unsigned long&);
	inline bistream& operator>>(float&);
	inline bistream& operator>>(double&);
	inline bistream& operator>>(long double&);
	inline bistream& operator>>(bool&);
	inline bistream& operator>>(char*);
	inline bistream& operator>>(unsigned char*);
	inline bistream& operator>>(const unsigned char*);

	bistream& read_buff(char* p, long len)
	{
		return read(p, len);
	}

private:
	bistream& read(char* p, long len) 
	{
		if(avail() < len)
			throw biosexception(biosexception::end);
		switch(len)
		{
		case 1:
			*p = *m_pCurr;
			break;
		case 2:
			*((unsigned short*)((unsigned char*)p)) = *((unsigned short*)((unsigned char*)m_pCurr));
			break;
		case 4:
			*((unsigned long*)((unsigned char*)p)) = *((unsigned long*)((unsigned char*)m_pCurr));
			break;
		default:
			memcpy(p, m_pCurr, len);
			break;
		}
		m_pCurr += len;
		return *this;
	}
};

///////////////////////////////////////////////////////////////////////
inline bostream& bostream::operator<<(char _c) { 
	write((const char*)&_c,sizeof(char)); 
	return *this; 
}
inline bostream& bostream::operator<<(unsigned char _c) {
	write((const char*)&_c,sizeof(unsigned char));
	return *this;
}
inline bostream& bostream::operator<<(signed char _c) {
	write((const char*)&_c,sizeof(signed char));
	return *this;
}
inline bostream& bostream::operator<<(short _s) {
	write((const char*)&_s,sizeof(short));
	return *this;
}
inline bostream& bostream::operator<<(unsigned short _s) {
	write((const char*)&_s,sizeof(unsigned short));
	return *this;
}
inline bostream& bostream::operator<<(int _i) {
	write((const char*)&_i,sizeof(int));
	return *this;
}
inline bostream& bostream::operator<<(unsigned int _i) {
	write((const char*)&_i,sizeof(unsigned int));
	return *this;
}
inline bostream& bostream::operator<<(long _l) {
	write((const char*)&_l,sizeof(long));
	return *this;
}
inline bostream& bostream::operator<<(unsigned long _l) {
	write((const char*)&_l,sizeof(unsigned long));
	return *this;
}
inline bostream& bostream::operator<<(float _f) {
	write((const char*)&_f,sizeof(float));
	return *this;
}
inline bostream& bostream::operator<<(double _d) {
	write((const char*)&_d,sizeof(double));
	return *this;
}
inline bostream& bostream::operator<<(long double _d) {
	write((const char*)&_d,sizeof(long double));
	return *this;
}
inline bostream& bostream::operator<<(bool _b) {
	write((const char*)&_b,sizeof(bool));
	return *this;
}
inline bostream& bostream::operator<<(const char* _s) {
	int  slen = (int)strlen(_s);
	if(slen < 255)
	{
		operator<<((unsigned char)slen);
	}
	else if(slen < 0xfffe)
	{
		// 这里根据字符的长度不同从前面开始填充数据，在读的的时候方便分辨
		// 因为读的时候会先读一个char如果=255则说明长度大于255继续读一个short
		// 如果 = 65535则说明长度大于short再读一个int
		operator<<((unsigned char)0xff);
		operator<<((unsigned short)slen);
	}
	else
	{
		operator<<((unsigned char)0xff);
		operator<<((unsigned short)0xffff);
		operator<<((unsigned long)slen);
	}
	write((const char*)_s,slen);
	return *this;
}
inline bostream& bostream::operator<<(char* _s) {
	return operator<<((const char*)_s) ;
}
inline bostream& bostream::operator<<(unsigned char* _s) {
	return operator<<((const char*)_s);
}
inline bostream& bostream::operator<<(const unsigned char* _s) {
	return operator<<((const char*)_s);
}


//////////////////////////////////////////////////////////////////////
inline bistream& bistream::operator>>(char& _c) {
	read(&_c,sizeof(char));
	return *this;
}
inline bistream& bistream::operator>>(signed char& _c) {
	read((char*)&_c,sizeof(signed char));
	return *this;
}
inline bistream& bistream::operator>>(unsigned char& _c) {
	read((char*)&_c,sizeof(unsigned char));
	return *this;
}
inline bistream& bistream::operator>>(short& _s) {
	read((char*)&_s,sizeof(short));
	return *this;
}
inline bistream& bistream::operator>>(unsigned short& _s) {
	read((char*)&_s,sizeof(unsigned short));
	return *this;
}
inline bistream& bistream::operator>>(int& _i) {
	read((char*)&_i,sizeof(int));
	return *this;
}
inline bistream& bistream::operator>>(unsigned int& _i) {
	read((char*)&_i,sizeof(unsigned int));
	return *this;
}
inline bistream& bistream::operator>>(long& _l) {
	read((char*)&_l,sizeof(long));
	return *this;
}
inline bistream& bistream::operator>>(unsigned long& _l) {
	read((char*)&_l,sizeof(unsigned long));
	return *this;
}
inline bistream& bistream::operator>>(float& _f) {
	read((char*)&_f,sizeof(float));
	return *this;
}
inline bistream& bistream::operator>>(double& _d) {
	read((char*)&_d,sizeof(double));
	return *this;
}
inline bistream& bistream::operator>>(long double& _d) {
	read((char*)&_d,sizeof(long double));
	return *this;
}
inline bistream& bistream::operator>>(bool& _b) {
	read((char*)&_b,sizeof(bool));
	return *this;
}
inline bistream& bistream::operator>>(char* _s) {
	unsigned char blen;
	operator>>(blen);
	if(blen < 0xff)
		return read(_s,blen);

	unsigned short wlen;
	operator>>(wlen);
	if(wlen < 0xfffe)
		return read(_s,wlen);

	unsigned long dwlen;
	operator>>(dwlen);
	return read(_s,dwlen);
}
inline bistream& bistream::operator>>(unsigned char* _s) {
	return operator>>((char*)_s);
}
inline bistream& bistream::operator>>(const unsigned char* _s) {
	return operator>>((char*)_s);
}
#endif
