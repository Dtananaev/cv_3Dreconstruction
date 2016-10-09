// CTensor
// A three-dimensional array
//
// Author: Thomas Brox

#ifndef CTENSOR_H
#define CTENSOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <CMatrix.h>
#include <NMath.h>

template <class T>
class CTensor {
public:
  // standard constructor
  inline CTensor();
  // constructor
  inline CTensor(const int aXSize, const int aYSize, const int aZSize);
  // copy constructor
  CTensor(const CTensor<T>& aCopyFrom);
  // constructor with implicit filling
  CTensor(const int aXSize, const int aYSize, const int aZSize, const T aFillValue);
  // destructor
  virtual ~CTensor();

  // Changes the size of the tensor, data will be lost
  void setSize(int aXSize, int aYSize, int aZSize);
  // Downsamples the tensor
  void downsample(int aNewXSize, int aNewYSize);
  // Upsamples the tensor
  void upsample(int aNewXSize, int aNewYSize);
  void upsampleBilinear(int aNewXSize, int aNewYSize);
  // Fills the tensor with the value aValue (see also operator =)
  void fill(const T aValue);
  // Copies a box from the tensor into aResult, the size of aResult will be adjusted
  void cut(CTensor<T>& aResult, int x1, int y1, int z1, int x2, int y2, int z2);
  // Copies aCopyFrom at a certain position of the tensor
  void paste(CTensor<T>& aCopyFrom, int ax, int ay, int az);
  // Mirrors the boundaries, aFrom is the distance from the boundaries where the pixels are copied from,
  // aTo is the distance from the boundaries they are copied to
  void mirrorLayers(int aFrom, int aTo);
  // Transforms the values so that they are all between aMin and aMax
  // aInitialMin/Max are initializations for seeking the minimum and maximum, change if your
  // data is not in this range or the data type T cannot hold these values
  void normalizeEach(T aMin, T aMax, T aInitialMin = -30000, T aInitialMax = 30000);
  void normalize(T aMin, T aMax, int aChannel, T aInitialMin = -30000, T aInitialMax = 30000);
  void normalize(T aMin, T aMax, T aInitialMin = -30000, T aInitialMax = 30000);
  // Draws a line into the image (only for mZSize = 3)
  void drawLine(int dStartX, int dStartY, int dEndX, int dEndY, T aValue1 = 255, T aValue2 = 255, T aValue3 = 255);

  // Computes Fourier transform and inverse Fourier transform
  // Image size has to be a power of 2, the two tensor channels comprise the real and imaginary part
  // The zero frequency is located in the center of the result
  void fft();
  void ifft();

  // Applies a similarity transform (translation, rotation, scaling) to the image
  void applySimilarityTransform(CTensor<T>& aWarped, CMatrix<bool>& aOutside, float tx, float ty, float cx, float cy, float phi, float scale);
  // Applies a homography (linear projective transformation) to the image
  void applyHomography(CTensor<T>& aWarped, CMatrix<bool>& aOutside, const CMatrix<float>& H);

  // Reads the tensor from a file in Mathematica format
  void readFromMathematicaFile(const char* aFilename);
  // Writes the tensor to a file in Mathematica format
  void writeToMathematicaFile(const char* aFilename);
  // Reads the tensor from a movie file in IM format
  void readFromIMFile(const char* aFilename);
  // Writes the tensor to a movie file in IM format
  void writeToIMFile(const char* aFilename);
  // Reads an image from a PGM file
  void readFromPGM(const char* aFilename);
  // Writes the tensor in PGM-Format
  void writeToPGM(const char* aFilename);
  // Extends a XxYx1 tensor to a XxYx3 tensor with three identical layers
  void makeColorTensor();
  // Reads a color image from a PPM file
  void readFromPPM(const char* aFilename);
  // Writes the tensor in PPM-Format
  void writeToPPM(const char* aFilename);
  // Reads the tensor from a PDM file
  void readFromPDM(const char* aFilename);
  // Writes the tensor in PDM-Format
  void writeToPDM(const char* aFilename, char aFeatureType);

  // Gives full access to tensor's values
  inline T& operator()(const int ax, const int ay, const int az) const;
  // Read access with bilinear interpolation
  CVector<T> operator()(const float ax, const float ay) const;
  // Fills the tensor with the value aValue (equivalent to fill())
  inline CTensor<T>& operator=(const T aValue);
  // Copies the tensor aCopyFrom to this tensor (size of tensor might change)
  CTensor<T>& operator=(const CTensor<T>& aCopyFrom);
  // Adds a tensor of same size
  CTensor<T>& operator+=(const CTensor<T>& aMatrix);
  // Adds a constant to the tensor
  CTensor<T>& operator+=(const T aValue);
  // Multiplication with a scalar
  CTensor<T>& operator*=(const T aValue);

  // Returns the minimum value
  T min() const;
  // Returns the maximum value
  T max() const;
  // Returns the average value
  T avg() const;
  // Returns the average value of a specific layer
  T avg(int az) const;
  // Gives access to the tensor's size
  inline int xSize() const;
  inline int ySize() const;
  inline int zSize() const;
  inline int size() const;
  // Returns the az layer of the tensor as matrix (slow and fast version)
  CMatrix<T> getMatrix(const int az) const;
  void getMatrix(CMatrix<T>& aMatrix, const int az) const;
  // Copies the matrix components of aMatrix into the az layer of the tensor
  void putMatrix(CMatrix<T>& aMatrix, const int az);
  // Gives access to the internal data representation (use sparingly)
  inline T* data() const;

  // Possible interpretations of the third tensor dimension for PDM format
  static const char cSpacial = 'S';
  static const char cVector = 'V';
  static const char cColor = 'C';
  static const char cSymmetricMatrix = 'Y';
protected:
  int mXSize,mYSize,mZSize;
  T *mData;
};

// Provides basic output functionality (only appropriate for very small tensors)
template <class T> std::ostream& operator<<(std::ostream& aStream, const CTensor<T>& aTensor);

// Exceptions thrown by CTensor-------------------------------------------------

// Thrown when one tries to access an element of a tensor which is out of
// the tensor's bounds
struct ETensorRangeOverflow {
  ETensorRangeOverflow(const int ax, const int ay, const int az) {
    using namespace std;
    cerr << "Exception ETensorRangeOverflow: x = " << ax << ", y = " << ay << ", z = " << az << endl;
  }
};

// Thrown when the size of a tensor does not match the needed size for a certain operation
struct ETensorIncompatibleSize {
  ETensorIncompatibleSize(int ax, int ay, int ax2, int ay2) {
    using namespace std;
    cerr << "Exception ETensorIncompatibleSize: x = " << ax << ":" << ax2;
    cerr << ", y = " << ay << ":" << ay2 << endl;
  }
  ETensorIncompatibleSize(int ax, int ay, int az) {
    std::cerr << "Exception ETensorIncompatibleTensorSize: x = " << ax << ", y = " << ay << ", z= " << az << std::endl;
  }
};

// I M P L E M E N T A T I O N --------------------------------------------
//
// You might wonder why there is implementation code in a header file.
// The reason is that not all C++ compilers yet manage separate compilation
// of templates. Inline functions cannot be compiled separately anyway.
// So in this case the whole implementation code is added to the header
// file.
// Users of CTensor should ignore everything that's beyond this line :)
// ------------------------------------------------------------------------

// P U B L I C ------------------------------------------------------------

// standard constructor
template <class T>
inline CTensor<T>::CTensor() {
  mData = 0;
}

// constructor
template <class T>
inline CTensor<T>::CTensor(const int aXSize, const int aYSize, const int aZSize)
  : mXSize(aXSize), mYSize(aYSize), mZSize(aZSize) {
  mData = new T[aXSize*aYSize*aZSize];
}

// copy constructor
template <class T>
CTensor<T>::CTensor(const CTensor<T>& aCopyFrom)
  : mXSize(aCopyFrom.mXSize), mYSize(aCopyFrom.mYSize), mZSize(aCopyFrom.mZSize) {
  int wholeSize = mXSize*mYSize*mZSize;
  mData = new T[wholeSize];
  for (register int i = 0; i < wholeSize; i++)
    mData[i] = aCopyFrom.mData[i];
}

// constructor with implicit filling
template <class T>
CTensor<T>::CTensor(const int aXSize, const int aYSize, const int aZSize, const T aFillValue)
  : mXSize(aXSize), mYSize(aYSize), mZSize(aZSize) {
  mData = new T[aXSize*aYSize*aZSize];
  fill(aFillValue);
}

// destructor
template <class T>
CTensor<T>::~CTensor() {
  delete [] mData;
}

// setSize
template <class T>
void CTensor<T>::setSize(int aXSize, int aYSize, int aZSize) {
  if (mData != 0) delete[] mData;
  mData = new T[aXSize*aYSize*aZSize];
  mXSize = aXSize;
  mYSize = aYSize;
  mZSize = aZSize;
}

//downsample
template <class T>
void CTensor<T>::downsample(int aNewXSize, int aNewYSize) {
  T* mData2 = new T[aNewXSize*aNewYSize*mZSize];
  int aSize = aNewXSize*aNewYSize;
  for (int z = 0; z < mZSize; z++) {
    CMatrix<T> aTemp(mXSize,mYSize);
    getMatrix(aTemp,z);
    aTemp.downsample(aNewXSize,aNewYSize);
    for (int i = 0; i < aSize; i++)
      mData2[i+z*aSize] = aTemp.data()[i];
  }
  delete[] mData;
  mData = mData2;
  mXSize = aNewXSize;
  mYSize = aNewYSize;
}

// upsample
template <class T>
void CTensor<T>::upsample(int aNewXSize, int aNewYSize) {
  T* mData2 = new T[aNewXSize*aNewYSize*mZSize];
  int aSize = aNewXSize*aNewYSize;
  for (int z = 0; z < mZSize; z++) {
    CMatrix<T> aTemp(mXSize,mYSize);
    getMatrix(aTemp,z);
    aTemp.upsample(aNewXSize,aNewYSize);
    for (int i = 0; i < aSize; i++)
      mData2[i+z*aSize] = aTemp.data()[i];
  }
  delete[] mData;
  mData = mData2;
  mXSize = aNewXSize;
  mYSize = aNewYSize;
}

// upsampleBilinear
template <class T>
void CTensor<T>::upsampleBilinear(int aNewXSize, int aNewYSize) {
  T* mData2 = new T[aNewXSize*aNewYSize*mZSize];
  int aSize = aNewXSize*aNewYSize;
  for (int z = 0; z < mZSize; z++) {
    CMatrix<T> aTemp(mXSize,mYSize);
    getMatrix(aTemp,z);
    aTemp.upsampleBilinear(aNewXSize,aNewYSize);
    for (int i = 0; i < aSize; i++)
      mData2[i+z*aSize] = aTemp.data()[i];
  }
  delete[] mData;
  mData = mData2;
  mXSize = aNewXSize;
  mYSize = aNewYSize;
}

// fill
template <class T>
void CTensor<T>::fill(const T aValue) {
  int wholeSize = mXSize*mYSize*mZSize;
  for (register int i = 0; i < wholeSize; i++)
    mData[i] = aValue;
}

// cut
template <class T>
void CTensor<T>::cut(CTensor<T>& aResult, int x1, int y1, int z1, int x2, int y2, int z2) {
  aResult.mXSize = x2-x1+1;
  aResult.mYSize = y2-y1+1;
  aResult.mZSize = z2-z1+1;
  delete[] aResult.mData;
  aResult.mData = new T[aResult.mXSize*aResult.mYSize*aResult.mZSize];
  for (int z = z1; z <= z2; z++)
    for (int y = y1; y <= y2; y++)
      for (int x = x1; x <= x2; x++)
        aResult(x-x1,y-y1,z-z1) = operator()(x,y,z);
}

// paste
template <class T>
void CTensor<T>::paste(CTensor<T>& aCopyFrom, int ax, int ay, int az) {
  for (int z = 0; z < aCopyFrom.zSize(); z++)
    for (int y = 0; y < aCopyFrom.ySize(); y++)
      for (int x = 0; x < aCopyFrom.xSize(); x++)
        operator()(ax+x,ay+y,az+z) = aCopyFrom(x,y,z);
}

// mirrorLayers
template <class T>
void CTensor<T>::mirrorLayers(int aFrom, int aTo) {
  for (int z = 0; z < mZSize; z++) {
    int aToXIndex = mXSize-aTo-1;
    int aToYIndex = mYSize-aTo-1;
    int aFromXIndex = mXSize-aFrom-1;
    int aFromYIndex = mYSize-aFrom-1;
    for (int y = aFrom; y <= aFromYIndex; y++) {
      operator()(aTo,y,z) = operator()(aFrom,y,z);
      operator()(aToXIndex,y,z) = operator()(aFromXIndex,y,z);
    }
    for (int x = aTo; x <= aToXIndex; x++) {
      operator()(x,aTo,z) = operator()(x,aFrom,z);
      operator()(x,aToYIndex,z) = operator()(x,aFromYIndex,z);
    }
  }
}

// normalize
template <class T>
void CTensor<T>::normalizeEach(T aMin, T aMax, T aInitialMin, T aInitialMax) {
  for (int k = 0; k < mZSize; k++)
    normalize(aMin,aMax,k,aInitialMin,aInitialMax);
}

template <class T>
void CTensor<T>::normalize(T aMin, T aMax, int aChannel, T aInitialMin, T aInitialMax) {
  int aChannelSize = mXSize*mYSize;
  T aCurrentMin = aInitialMax;
  T aCurrentMax = aInitialMin;
  int aIndex = aChannelSize*aChannel;
  for (int i = 0; i < aChannelSize; i++) {
    if (mData[aIndex] > aCurrentMax) aCurrentMax = mData[aIndex];
    else if (mData[aIndex] < aCurrentMin) aCurrentMin = mData[aIndex];
    aIndex++;
  }
  T aTemp1 = aCurrentMin - aMin;
  T aTemp2 = (aCurrentMax-aCurrentMin);
  if (aTemp2 == 0) aTemp2 = 1;
  else aTemp2 = (aMax-aMin)/aTemp2;
  aIndex = aChannelSize*aChannel;
  for (int i = 0; i < aChannelSize; i++) {
    mData[aIndex] -= aTemp1;
    mData[aIndex] *= aTemp2;
    aIndex++;
  }
}

// drawLine
template <class T>
void CTensor<T>::drawLine(int dStartX, int dStartY, int dEndX, int dEndY, T aValue1, T aValue2, T aValue3) {
  int aOffset1 = mXSize*mYSize;
  int aOffset2 = 2*aOffset1;
	// vertical line
	if (dStartX == dEndX) {
    if (dStartX < 0 || dStartX >= mXSize)	return;
		int x = dStartX;
		if (dStartY < dEndY) {
			for (int y = dStartY; y <= dEndY; y++)
				if (y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
  	}
		else {
			for (int y = dStartY; y >= dEndY; y--)
				if (y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
    }
    return;
  }
	// horizontal line
	if (dStartY == dEndY) {
    if (dStartY < 0 || dStartY >= mYSize) return;
 		int y = dStartY;
		if (dStartX < dEndX) {
			for (int x = dStartX; x <= dEndX; x++)
				if (x >= 0 && x < mXSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
  	}
		else {
			for (int x = dStartX; x >= dEndX; x--)
				if (x >= 0 && x < mXSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
    }
    return;
  }
  float m = float(dStartY - dEndY) / float(dStartX - dEndX);
  float invm = 1.0/m;
  if (fabs(m) > 1.0) {
    if (dEndY > dStartY) {
      for (int y = dStartY; y <= dEndY; y++) {
        int x = (int)(0.5+dStartX+(y-dStartY)*invm);
        if (x >= 0 && x < mXSize &&	y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
      }
    }
    else {
      for (int y = dStartY; y >= dEndY; y--) {
        int x = (int)(0.5+dStartX+(y-dStartY)*invm);
        if (x >= 0 && x < mXSize &&	y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
      }
    }
  }
  else {
    if (dEndX > dStartX) {
      for (int x = dStartX; x <= dEndX; x++) {
        int y = (int)(0.5+dStartY+(x-dStartX)*m);
        if (x >= 0 && x < mXSize &&	y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
      }
    }
    else {
      for (int x = dStartX; x >= dEndX; x--) {
        int y = (int)(0.5+dStartY+(x-dStartX)*m);
        if (x >= 0 && x < mXSize &&	y >= 0 && y < mYSize) {
          mData[x+y*mXSize] = aValue1;
          mData[x+y*mXSize+aOffset1] = aValue2;
          mData[x+y*mXSize+aOffset2] = aValue3;
        }
      }
    }
  }
}

template <class T>
void CTensor<T>::normalize(T aMin, T aMax, T aInitialMin, T aInitialMax) {
  int aSize = mXSize*mYSize*mZSize;
  T aCurrentMin = aInitialMax;
  T aCurrentMax = aInitialMin;
  for (int i = 0; i < aSize; i++) {
    if (mData[i] > aCurrentMax) aCurrentMax = mData[i];
    else if (mData[i] < aCurrentMin) aCurrentMin = mData[i];
  }
  T aTemp1 = aCurrentMin - aMin;
  T aTemp2 = (aCurrentMax-aCurrentMin);
  if (aTemp2 == 0) aTemp2 = 1;
  else aTemp2 = (aMax-aMin)/aTemp2;
  for (int i = 0; i < aSize; i++) {
    mData[i] -= aTemp1;
    mData[i] *= aTemp2;
  }
}

// applySimilarityTransform
template <class T>
void CTensor<T>::applySimilarityTransform(CTensor<T>& aWarped, CMatrix<bool>& aOutside, float tx, float ty, float cx, float cy, float phi, float scale) {
  float cosphi = scale*cos(phi);
  float sinphi = scale*sin(phi);
  int aSize = mXSize*mYSize;
  int aWarpedSize = aWarped.xSize()*aWarped.ySize();
  float ctx = cx+tx-cx*cosphi+cy*sinphi;
  float cty = cy+ty-cy*cosphi-cx*sinphi;
  aOutside = false;
  int i = 0;
  for (int y = 0; y < aWarped.ySize(); y++)
    for (int x = 0; x < aWarped.xSize(); x++,i++) {
      float xf = x; float yf = y;
      float ax = xf*cosphi-yf*sinphi+ctx;
      float ay = yf*cosphi+xf*sinphi+cty;
      int x1 = (int)ax; int y1 = (int)ay;
      float alphaX = ax-x1; float alphaY = ay-y1;
      float betaX = 1.0-alphaX; float betaY = 1.0-alphaY;
      if (x1 < 0 || y1 < 0 || x1+1 >= mXSize || y1+1 >= mYSize) aOutside.data()[i] = true;
      else {
        int j = y1*mXSize+x1;
        for (int k = 0; k < mZSize; k++) {
          float a = betaX*mData[j]       +alphaX*mData[j+1];
          float b = betaX*mData[j+mXSize]+alphaX*mData[j+1+mXSize];
          aWarped.data()[i+k*aWarpedSize] = betaY*a+alphaY*b;
          j += aSize;
        }
      }
    }
}

// applyHomography
template <class T>
void CTensor<T>::applyHomography(CTensor<T>& aWarped, CMatrix<bool>& aOutside, const CMatrix<float>& H) {
  int aSize = mXSize*mYSize;
  int aWarpedSize = aWarped.xSize()*aWarped.ySize();
  aOutside = false;
  int i = 0;
  for (int y = 0; y < aWarped.ySize(); y++)
    for (int x = 0; x < aWarped.xSize(); x++,i++) {
      float xf = x; float yf = y;
      float ax = H.data()[0]*xf+H.data()[1]*yf+H.data()[2];
      float ay = H.data()[3]*xf+H.data()[4]*yf+H.data()[5];
      float az = H.data()[6]*xf+H.data()[7]*yf+H.data()[8];
      float invaz = 1.0/az;
      ax *= invaz; ay *= invaz;
      int x1 = (int)ax; int y1 = (int)ay;
      float alphaX = ax-x1; float alphaY = ay-y1;
      float betaX = 1.0-alphaX; float betaY = 1.0-alphaY;
      if (x1 < 0 || y1 < 0 || x1+1 >= mXSize || y1+1 >= mYSize) aOutside.data()[i] = true;
      else {
        int j = y1*mXSize+x1;
        for (int k = 0; k < mZSize; k++) {
          float a = betaX*mData[j]       +alphaX*mData[j+1];
          float b = betaX*mData[j+mXSize]+alphaX*mData[j+1+mXSize];
          aWarped.data()[i+k*aWarpedSize] = betaY*a+alphaY*b;
          j += aSize;
        }
      }
    }
}

// -----------------------------------------------------------------------------
// FFT (from Takuya OOURA FFT package)
// -----------------------------------------------------------------------------

void bitrv2(int n, int *ip, double *a) {
  int j, j1, k, k1, l, m, m2;
  double xr, xi;
  ip[0] = 0;
  l = n;
  m = 1;
  while ((m << 2) < l) {
    l >>= 1;
    for (j = 0; j <= m - 1; j++)
      ip[m + j] = ip[j] + l;
    m <<= 1;
  }
  if ((m << 2) > l) {
    for (k = 1; k <= m - 1; k++)
      for (j = 0; j <= k - 1; j++) {
        j1 = (j << 1) + ip[k];
        k1 = (k << 1) + ip[j];
        xr = a[j1];
        xi = a[j1 + 1];
        a[j1] = a[k1];
        a[j1 + 1] = a[k1 + 1];
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
  }
  else {
    m2 = m << 1;
    for (k = 1; k <= m - 1; k++)
      for (j = 0; j <= k - 1; j++) {
        j1 = (j << 1) + ip[k];
        k1 = (k << 1) + ip[j];
        xr = a[j1];
        xi = a[j1 + 1];
        a[j1] = a[k1];
        a[j1 + 1] = a[k1 + 1];
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += m2;
        xr = a[j1];
        xi = a[j1 + 1];
        a[j1] = a[k1];
        a[j1 + 1] = a[k1 + 1];
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
  }
}

void makewt(int nw, int *ip, double *w) {
  int nwh, j;
  double delta, x, y;
  ip[0] = nw;
  ip[1] = 1;
  if (nw > 2) {
    nwh = nw >> 1;
    delta = atan(1.0) / nwh;
    w[0] = 1;
    w[1] = 0;
    w[nwh] = cos(delta * nwh);
    w[nwh + 1] = w[nwh];
    for (j = 2; j <= nwh - 2; j += 2) {
      x = cos(delta * j);
      y = sin(delta * j);
      w[j] = x;
      w[j + 1] = y;
      w[nw - j] = y;
      w[nw - j + 1] = x;
    }
    bitrv2(nw, ip + 2, w);
  }
}

void cftbsub(int n, double *a, double *w) {
  int j, j1, j2, j3, k, k1, ks, l, m;
  double wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
  l = 2;
  while ((l << 1) < n) {
    m = l << 2;
    for (j = 0; j <= l - 2; j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = a[j + 1] + a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = a[j + 1] - a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i + x2i;
      a[j2] = x0r - x2r;
      a[j2 + 1] = x0i - x2i;
      a[j1] = x1r - x3i;
      a[j1 + 1] = x1i + x3r;
      a[j3] = x1r + x3i;
      a[j3 + 1] = x1i - x3r;
    }
    if (m < n) {
      wk1r = w[2];
      for (j = m; j <= l + m - 2; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x2i - x0i;
        a[j2 + 1] = x0r - x2r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j1] = wk1r * (x0r - x0i);
        a[j1 + 1] = wk1r * (x0r + x0i);
        x0r = x3i + x1r;
        x0i = x3r - x1i;
        a[j3] = wk1r * (x0i - x0r);
        a[j3 + 1] = wk1r * (x0i + x0r);
      }
      k1 = 1;
      ks = -1;
      for (k = (m << 1); k <= n - m; k += m) {
        k1++;
        ks = -ks;
        wk1r = w[k1 << 1];
        wk1i = w[(k1 << 1) + 1];
        wk2r = ks * w[k1];
        wk2i = w[k1 + ks];
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        for (j = k; j <= l + k - 2; j += 2) {
          j1 = j + l;
          j2 = j1 + l;
          j3 = j2 + l;
          x0r = a[j] + a[j1];
          x0i = a[j + 1] + a[j1 + 1];
          x1r = a[j] - a[j1];
          x1i = a[j + 1] - a[j1 + 1];
          x2r = a[j2] + a[j3];
          x2i = a[j2 + 1] + a[j3 + 1];
          x3r = a[j2] - a[j3];
          x3i = a[j2 + 1] - a[j3 + 1];
          a[j] = x0r + x2r;
          a[j + 1] = x0i + x2i;
          x0r -= x2r;
          x0i -= x2i;
          a[j2] = wk2r * x0r - wk2i * x0i;
          a[j2 + 1] = wk2r * x0i + wk2i * x0r;
          x0r = x1r - x3i;
          x0i = x1i + x3r;
          a[j1] = wk1r * x0r - wk1i * x0i;
          a[j1 + 1] = wk1r * x0i + wk1i * x0r;
          x0r = x1r + x3i;
          x0i = x1i - x3r;
          a[j3] = wk3r * x0r - wk3i * x0i;
          a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
      }
    }
    l = m;
  }
  if (l < n) {
    for (j = 0; j <= l - 2; j += 2) {
      j1 = j + l;
      x0r = a[j] - a[j1];
      x0i = a[j + 1] - a[j1 + 1];
      a[j] += a[j1];
      a[j + 1] += a[j1 + 1];
      a[j1] = x0r;
      a[j1 + 1] = x0i;
    }
  }
}

void cftfsub(int n, double *a, double *w) {
  int j, j1, j2, j3, k, k1, ks, l, m;
  double wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

  l = 2;
  while ((l << 1) < n) {
    m = l << 2;
    for (j = 0; j <= l - 2; j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = a[j + 1] + a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = a[j + 1] - a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i + x2i;
      a[j2] = x0r - x2r;
      a[j2 + 1] = x0i - x2i;
      a[j1] = x1r + x3i;
      a[j1 + 1] = x1i - x3r;
      a[j3] = x1r - x3i;
      a[j3 + 1] = x1i + x3r;
    }
    if (m < n) {
      wk1r = w[2];
      for (j = m; j <= l + m - 2; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x0i - x2i;
        a[j2 + 1] = x2r - x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j1] = wk1r * (x0i + x0r);
        a[j1 + 1] = wk1r * (x0i - x0r);
        x0r = x3i - x1r;
        x0i = x3r + x1i;
        a[j3] = wk1r * (x0r + x0i);
        a[j3 + 1] = wk1r * (x0r - x0i);
      }
      k1 = 1;
      ks = -1;
      for (k = (m << 1); k <= n - m; k += m) {
        k1++;
        ks = -ks;
        wk1r = w[k1 << 1];
        wk1i = w[(k1 << 1) + 1];
        wk2r = ks * w[k1];
        wk2i = w[k1 + ks];
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        for (j = k; j <= l + k - 2; j += 2) {
          j1 = j + l;
          j2 = j1 + l;
          j3 = j2 + l;
          x0r = a[j] + a[j1];
          x0i = a[j + 1] + a[j1 + 1];
          x1r = a[j] - a[j1];
          x1i = a[j + 1] - a[j1 + 1];
          x2r = a[j2] + a[j3];
          x2i = a[j2 + 1] + a[j3 + 1];
          x3r = a[j2] - a[j3];
          x3i = a[j2 + 1] - a[j3 + 1];
          a[j] = x0r + x2r;
          a[j + 1] = x0i + x2i;
          x0r -= x2r;
          x0i -= x2i;
          a[j2] = wk2r * x0r + wk2i * x0i;
          a[j2 + 1] = wk2r * x0i - wk2i * x0r;
          x0r = x1r + x3i;
          x0i = x1i - x3r;
          a[j1] = wk1r * x0r + wk1i * x0i;
          a[j1 + 1] = wk1r * x0i - wk1i * x0r;
          x0r = x1r - x3i;
          x0i = x1i + x3r;
          a[j3] = wk3r * x0r + wk3i * x0i;
          a[j3 + 1] = wk3r * x0i - wk3i * x0r;
        }
      }
    }
    l = m;
  }
  if (l < n) {
    for (j = 0; j <= l - 2; j += 2) {
      j1 = j + l;
      x0r = a[j] - a[j1];
      x0i = a[j + 1] - a[j1 + 1];
      a[j] += a[j1];
      a[j + 1] += a[j1 + 1];
      a[j1] = x0r;
      a[j1 + 1] = x0i;
    }
  }
}

void cdft(int n, int isgn, double *a, int *ip, double *w) {
  void cftbsub(int n, double *a, double *w);
  void cftfsub(int n, double *a, double *w);

  if (n > (ip[0] << 2)) makewt(n >> 2, ip, w);
  if (n > 4) bitrv2(n, ip + 2, a);
  if (isgn < 0) cftfsub(n, a, w);
  else cftbsub(n, a, w);
}

void cdft2d(int n1, int n2, int isgn, double **a, double *t, int *ip, double *w) {
  int n, i, j, i2;
  n = n1 << 1;
  if (n < n2)  n = n2;
  if (n > (ip[0] << 2)) makewt(n >> 2, ip, w);
  for (i = 0; i <= n1 - 1; i++)
    cdft(n2, isgn, a[i], ip, w);
  for (j = 0; j <= n2 - 2; j += 2) {
    for (i = 0; i <= n1 - 1; i++) {
      i2 = i << 1;
      t[i2] = a[i][j];
      t[i2 + 1] = a[i][j + 1];
    }
    cdft(n1 << 1, isgn, t, ip, w);
    for (i = 0; i <= n1 - 1; i++) {
      i2 = i << 1;
      a[i][j] = t[i2];
      a[i][j + 1] = t[i2 + 1];
    }
  }
}

template <class T>
void CTensor<T>::fft() {
  int n1 = mXSize;
  int n2 = mYSize;
  // Reserve memory
  double** a = new double*[n1];
  for (int i = 0; i < n1; i++)
    a[i] = new double[2*n2];
  double* t = new double[2*n1];
  int* ip = new int[2+NMath::max(n1,n2)];
  ip[0] = 0;
  double* w = new double[NMath::max(n1/2,n2/2)];
  // Apply FFT to data
  for (int y = 0; y < mYSize; y++)
    for (int x = 0; x < mXSize; x++) {
      a[x][2*y] = operator()(x,y,0);
      a[x][2*y+1] = operator()(x,y,1);
    }
  cdft2d(n1,2*n2,1,a,t,ip,w);
  int n12 = n1/2;
  int n22 = n2/2;
  for (int y = 0; y < n22; y++)
    for (int x = 0; x < n12; x++) {
      operator()(n12-1-x,n22-1-y,0) = a[x][2*y];
      operator()(n12-1-x,n22-1-y,1) = a[x][2*y+1];
      operator()(n1-1-x,n22-1-y,0) = a[x+n12][2*y];
      operator()(n1-1-x,n22-1-y,1) = a[x+n12][2*y+1];
      operator()(n12-1-x,n2-1-y,0) = a[x][2*(y+n22)];
      operator()(n12-1-x,n2-1-y,1) = a[x][2*(y+n22)+1];
      operator()(n1-1-x,n2-1-y,0) = a[x+n12][2*(y+n22)];
      operator()(n1-1-x,n2-1-y,1) = a[x+n12][2*(y+n22)+1];
    }
  // Release memory
  delete[] w;
  delete[] ip;
  delete[] t;
  for (int i = 0; i < n1; i++)
    delete[] a[i];
  delete[] a;
}

template <class T>
void CTensor<T>::ifft() {
  int n1 = mXSize;
  int n2 = mYSize;
  // Reserve memory
  double** a = new double*[n1];
  for (int i = 0; i < n1; i++)
    a[i] = new double[2*n2];
  double* t = new double[2*n1];
  int* ip = new int[2+NMath::max(n1,n2)];
  ip[0] = 0;
  double* w = new double[NMath::max(n1/2,n2/2)];
  // Apply inverse FFT to data
  int n12 = n1/2;
  int n22 = n2/2;
  for (int y = 0; y < n22; y++)
    for (int x = 0; x < n12; x++) {
      a[x][2*y] = operator()(n12-1-x,n22-1-y,0);
      a[x][2*y+1] = operator()(n12-1-x,n22-1-y,1);
      a[x+n12][2*y] = operator()(n1-1-x,n22-1-y,0);
      a[x+n12][2*y+1] = operator()(n1-1-x,n22-1-y,1);
      a[x][2*(y+n22)] = operator()(n12-1-x,n2-1-y,0);
      a[x][2*(y+n22)+1] = operator()(n12-1-x,n2-1-y,1);
      a[x+n12][2*(y+n22)] = operator()(n1-1-x,n2-1-y,0);
      a[x+n12][2*(y+n22)+1] = operator()(n1-1-x,n2-1-y,1);
    }
  cdft2d(n1,2*n2,-1,a,t,ip,w);
  double invSize = 1.0/(n1*n2);
  for (int y = 0; y < mYSize; y++)
    for (int x = 0; x < mXSize; x++) {
      operator()(x,y,0) = invSize*a[x][2*y];
      operator()(x,y,1) = invSize*a[x][2*y+1];
    }
  // Release memory
  delete[] w;
  //delete[] ip;
  delete[] t;
  for (int i = 0; i < n1; i++)
    delete[] a[i];
  delete[] a;
}

// -----------------------------------------------------------------------------
// File I/O
// -----------------------------------------------------------------------------

// readFromMathematicaFile
template <class T>
void CTensor<T>::readFromMathematicaFile(const char* aFilename) {
  using namespace std;
  // Read the whole file and store data in aData
  // Ignore blanks, tabs and lines
  // Also ignore Mathematica comments (* ... *)
  ifstream aStream(aFilename);
  string aData;
  char aChar;
  bool aBracketFound = false;
  bool aStarFound = false;
  bool aCommentFound = false;
  while (aStream.get(aChar))
    if (aChar != ' ' && aChar != '\t' && aChar != '\n') {
      if (aCommentFound) {
        if (!aStarFound && aChar == '*') aStarFound = true;
        else {
          if (aStarFound && aChar == ')') aCommentFound = false;
          aStarFound = false;
        }
      }
      else {
        if (!aBracketFound && aChar == '(') aBracketFound = true;
        else {
          if (aBracketFound && aChar == '*') aCommentFound = true;
          else aData += aChar;
          aBracketFound = false;
        }
      }
    }
  // Count the number of braces and double braces to figure out z- and y-Size of tensor
  int aDoubleBraceCount = 0;
  int aBraceCount = 0;
  int aPos = 0;
  while ((aPos = aData.find_first_of('{',aPos)+1) > 0) {
    aBraceCount++;
    if (aData[aPos] == '{' && aData[aPos+1] != '{') aDoubleBraceCount++;
  }
  // Count the number of commas in the first section to figure out xSize of tensor
  int aCommaCount = 0;
  aPos = 0;
  while (aData[aPos] != '}') {
    if (aData[aPos] == ',') aCommaCount++;
    aPos++;
  }
  // Adapt size of tensor
  if (mData != 0) delete[] mData;
  mXSize = aCommaCount+1;
  mYSize = (aBraceCount-1-aDoubleBraceCount) / aDoubleBraceCount;
  mZSize = aDoubleBraceCount;
  mData = new T[mXSize*mYSize*mZSize];
  // Analyse file ---------------
  aPos = 0;
  if (aData[aPos] != '{') throw EInvalidFileFormat("Mathematica");
  aPos++;
  for (int z = 0; z < mZSize; z++) {
    if (aData[aPos] != '{') throw EInvalidFileFormat("Mathematica");
    aPos++;
    for (int y = 0; y < mYSize; y++) {
      if (aData[aPos] != '{') throw EInvalidFileFormat("Mathematica");
      aPos++;
      for (int x = 0; x < mXSize; x++) {
        int oldPos = aPos;
        if (x+1 < mXSize) aPos = aData.find_first_of(',',aPos);
        else aPos = aData.find_first_of('}',aPos);
        #ifdef GNU_COMPILER
        string s = aData.substr(oldPos,aPos-oldPos);
        istrstream is(s.c_str());
        #else
        string s = aData.substr(oldPos,aPos-oldPos);
        istringstream is(s);
        #endif
        T aItem;
        is >> aItem;
        operator()(x,y,z) = aItem;
        aPos++;
      }
      if (y+1 < mYSize) {
        if (aData[aPos] != ',') throw EInvalidFileFormat("Mathematica");
        aPos++;
        while (aData[aPos] != '{')
          aPos++;
      }
    }
    aPos++;
    if (z+1 < mZSize) {
      if (aData[aPos] != ',') throw EInvalidFileFormat("Mathematica");
      aPos++;
      while (aData[aPos] != '{')
        aPos++;
    }
  }
}

// writeToMathematicaFile
template <class T>
void CTensor<T>::writeToMathematicaFile(const char* aFilename) {
  using namespace std;
  ofstream aStream(aFilename);
  aStream << '{';
  for (int z = 0; z < mZSize; z++) {
    aStream << '{';
    for (int y = 0; y < mYSize; y++) {
      aStream << '{';
      for (int x = 0; x < mXSize; x++) {
        aStream << operator()(x,y,z);
        if (x+1 < mXSize) aStream << ',';
      }
      aStream << '}';
      if (y+1 < mYSize) aStream << ",\n";
    }
    aStream << '}';
    if (z+1 < mZSize) aStream << ",\n";
  }
  aStream << '}';
}

// readFromIMFile
template <class T>
void CTensor<T>::readFromIMFile(const char* aFilename) {
  FILE *aStream;
  aStream = fopen(aFilename,"rb");
  // Read image data
  for (int i = 0; i < mXSize*mYSize*mZSize; i++)
    mData[i] = getc(aStream);
  fclose(aStream);
}

// writeToIMFile
template <class T>
void CTensor<T>::writeToIMFile(const char *aFilename) {
  FILE *aStream;
  aStream = fopen(aFilename,"wb");
  // write data
  for (int i = 0; i < mXSize*mYSize*mZSize; i++) {
    char dummy = (char)mData[i];
    fwrite(&dummy,1,1,aStream);
  }
  fclose(aStream);
}

// readFromPGM
template <class T>
void CTensor<T>::readFromPGM(const char* aFilename) {
  FILE *aStream;
  aStream = fopen(aFilename,"rb");
  int dummy;
  // Find beginning of file (P5)
  while (getc(aStream) != 'P');
  if (getc(aStream) != '5') throw EInvalidFileFormat("PGM");
  while (getc(aStream) != '\n');
  // Remove comments and empty lines
  dummy = getc(aStream);
  while (dummy == '#') {
    while (getc(aStream) != '\n');
    dummy = getc(aStream);
  }
  while (dummy == '\n')
    dummy = getc(aStream); 
  // Read image size
  mXSize = dummy-48;
  while ((dummy = getc(aStream)) >= 48 && dummy < 58)
    mXSize = 10*mXSize+dummy-48;
  while ((dummy = getc(aStream)) < 48 || dummy >= 58);
  mYSize = dummy-48;
  while ((dummy = getc(aStream)) >= 48 && dummy < 58)
    mYSize = 10*mYSize+dummy-48;
  mZSize = 1;
  if (dummy != '\n') while (getc(aStream) != '\n');
  while (getc(aStream) != '\n');
  // Adjust size of data structure
  delete[] mData;
  mData = new T[mXSize*mYSize];
  // Read image data
  for (int i = 0; i < mXSize*mYSize; i++)
    mData[i] = getc(aStream);
  fclose(aStream);
}

// writeToPGM
template <class T>
void CTensor<T>::writeToPGM(const char* aFilename) {
  int rows = (int)floor(sqrt(mZSize));
  int cols = (int)ceil(mZSize*1.0/rows);
  FILE* outimage = fopen(aFilename, "wb");
  fprintf(outimage, "P5 \n");
  fprintf(outimage, "%ld %ld \n255\n", cols*mXSize,rows*mYSize);
  for (int r = 0; r < rows; r++)
    for (int y = 0; y < mYSize; y++)
      for (int c = 0; c < cols; c++)
        for (int x = 0; x < mXSize; x++) {
          unsigned char aHelp;
          if (r*cols+c >= mZSize) aHelp = 0;
          else aHelp = (unsigned char)operator()(x,y,r*cols+c);
          fwrite (&aHelp, sizeof(unsigned char), 1, outimage);
        }
  fclose(outimage);
}

// makeColorTensor
template <class T>
void CTensor<T>::makeColorTensor() {
  if (mZSize != 1) return;
  int aSize = mXSize*mYSize;
  int a2Size = 2*aSize;
  T* aNewData = new T[aSize*3];
  for (int i = 0; i < aSize; i++)
    aNewData[i] = aNewData[i+aSize] = aNewData[i+a2Size] = mData[i];
  mZSize = 3;
  delete[] mData;
  mData = aNewData;
}

// readFromPPM
template <class T>
void CTensor<T>::readFromPPM(const char* aFilename) {
  FILE *aStream;
  aStream = fopen(aFilename,"rb");
  if (aStream == 0) std::cerr << "File not found: " << aFilename << std::endl;
  int dummy;
  // Find beginning of file (P6)
  while (getc(aStream) != 'P');
  if (getc(aStream) != '6') throw EInvalidFileFormat("PPM");
  while (getc(aStream) != '\n');
  // Remove comments and empty lines
  dummy = getc(aStream);
  while (dummy == '#') {
    while (getc(aStream) != '\n');
    dummy = getc(aStream);
  }
  while (dummy == '\n')
    dummy = getc(aStream); 
  // Read image size
  mXSize = dummy-48;
  while ((dummy = getc(aStream)) >= 48 && dummy < 58)
    mXSize = 10*mXSize+dummy-48;
  while ((dummy = getc(aStream)) < 48 || dummy >= 58);
  mYSize = dummy-48;
  while ((dummy = getc(aStream)) >= 48 && dummy < 58)
    mYSize = 10*mYSize+dummy-48;
  mZSize = 3;
  if (dummy != '\n') while (getc(aStream) != '\n');
  while (getc(aStream) != '\n');
  // Adjust size of data structure
  delete[] mData;
  mData = new T[mXSize*mYSize*3];
  // Read image data
  int aSize = mXSize*mYSize;
  int aSizeTwice = aSize+aSize;
  for (int i = 0; i < aSize; i++) {
    mData[i] = getc(aStream);
    mData[i+aSize] = getc(aStream);
    mData[i+aSizeTwice] = getc(aStream);
  }
  fclose(aStream);
}

// writeToPPM
template <class T>
void CTensor<T>::writeToPPM(const char* aFilename) {
  FILE* outimage = fopen(aFilename, "wb");
  fprintf(outimage, "P6 \n");
  fprintf(outimage, "%ld %ld \n255\n", mXSize,mYSize);
  for (int y = 0; y < mYSize; y++)
    for (int x = 0; x < mXSize; x++) {
      unsigned char aHelp = (unsigned char)operator()(x,y,0);
      fwrite (&aHelp, sizeof(unsigned char), 1, outimage);
      aHelp = (unsigned char)operator()(x,y,1);
      fwrite (&aHelp, sizeof(unsigned char), 1, outimage);
      aHelp = (unsigned char)operator()(x,y,2);
      fwrite (&aHelp, sizeof(unsigned char), 1, outimage);
    }
  fclose(outimage);
}

// readFromPDM
template <class T>
void CTensor<T>::readFromPDM(const char* aFilename) {
  std::ifstream aStream(aFilename);
  std::string s;
  // Read header
  aStream >> s;
  if (s != "P9") throw EInvalidFileFormat("PDM");
  char aFeatureType;
  aStream >> aFeatureType;
  aStream >> s;
  aStream >> mXSize;
  aStream >> mYSize;
  aStream >> mZSize;
  aStream >> s;
  // Adjust size of data structure
  delete[] mData;
  mData = new T[mXSize*mYSize*mZSize];
  // Read data
  for (int i = 0; i < mXSize*mYSize*mZSize; i++)
    aStream >> mData[i];
}

// writeToPDM
template <class T>
void CTensor<T>::writeToPDM(const char* aFilename, char aFeatureType) {
  std::ofstream aStream(aFilename);
  // write header
  aStream << "P9" << std::endl;
  aStream << aFeatureType << "SS" << std::endl;
  aStream << mZSize << ' ' << mYSize << ' ' << mXSize << std::endl;
  aStream << "F" << std::endl;
  // write data
  for (int i = 0; i < mXSize*mYSize*mZSize; i++) {
    aStream << mData[i];
    if (i % 8 == 0) aStream << std::endl;
    else aStream << ' ';
  }
}

// operator ()
template <class T>
inline T& CTensor<T>::operator()(const int ax, const int ay, const int az) const {
  #ifdef _DEBUG
    if (ax >= mXSize || ay >= mYSize || az >= mZSize || ax < 0 || ay < 0 || az < 0)
      throw ETensorRangeOverflow(ax,ay,az);
  #endif
  return mData[mXSize*(mYSize*az+ay)+ax];
}

template <class T>
CVector<T> CTensor<T>::operator()(const float ax, const float ay) const {
  CVector<T> aResult(mZSize);
  int x1 = (int)ax;
  int y1 = (int)ay;
  int x2 = x1+1;
  int y2 = y1+1;
  #ifdef _DEBUG
  if (x2 >= mXSize || y2 >= mYSize || x1 < 0 || y1 < 0) throw ETensorRangeOverflow(ax,ay,0);
  #endif
  float alphaX = ax-x1; float alphaXTrans = 1.0-alphaX;
  float alphaY = ay-y1; float alphaYTrans = 1.0-alphaY;
  for (int k = 0; k < mZSize; k++) {
    float a = alphaXTrans*operator()(x1,y1,k)+alphaX*operator()(x2,y1,k);
    float b = alphaXTrans*operator()(x1,y2,k)+alphaX*operator()(x2,y2,k);
    aResult(k) = alphaYTrans*a+alphaY*b;
  }
  return aResult;
}

// operator =
template <class T>
inline CTensor<T>& CTensor<T>::operator=(const T aValue) {
  fill(aValue);
  return *this;
}

template <class T>
CTensor<T>& CTensor<T>::operator=(const CTensor<T>& aCopyFrom) {
  if (this != &aCopyFrom) {
    delete[] mData;
    mXSize = aCopyFrom.mXSize;
    mYSize = aCopyFrom.mYSize;
    mZSize = aCopyFrom.mZSize;
    int wholeSize = mXSize*mYSize*mZSize;
    mData = new T[wholeSize];
    for (register int i = 0; i < wholeSize; i++)
      mData[i] = aCopyFrom.mData[i];
  }
  return *this;
}

// operator +=
template <class T>
CTensor<T>& CTensor<T>::operator+=(const CTensor<T>& aTensor) {
  #ifdef _DEBUG
  if (mXSize != aTensor.mXSize || mYSize != aTensor.mYSize || mZSize != aTensor.mZSize)
    throw ETensorIncompatibleSize(mXSize,mYSize,mZSize);
  #endif
  int wholeSize = size();
  for (int i = 0; i < wholeSize; i++)
    mData[i] += aTensor.mData[i];
  return *this;
}

// operator +=
template <class T>
CTensor<T>& CTensor<T>::operator+=(const T aValue) {
  int wholeSize = mXSize*mYSize*mZSize;
  for (int i = 0; i < wholeSize; i++)
    mData[i] += aValue;
  return *this;
}

// operator *=
template <class T>
CTensor<T>& CTensor<T>::operator*=(const T aValue) {
  int wholeSize = mXSize*mYSize*mZSize;
  for (int i = 0; i < wholeSize; i++)
    mData[i] *= aValue;
  return *this;
}

// min
template <class T>
T CTensor<T>::min() const {
  T aMin = mData[0];
  int aSize = mXSize*mYSize*mZSize;
  for (int i = 1; i < aSize; i++)
    if (mData[i] < aMin) aMin = mData[i];
  return aMin;
}

// max
template <class T>
T CTensor<T>::max() const {
  T aMax = mData[0];
  int aSize = mXSize*mYSize*mZSize;
  for (int i = 1; i < aSize; i++)
    if (mData[i] > aMax) aMax = mData[i];
  return aMax;
}

// avg
template <class T>
T CTensor<T>::avg() const {
  T aAvg = 0;
  for (int z = 0; z < mZSize; z++)
    aAvg += avg(z);
  return aAvg/mZSize;
}

template <class T>
T CTensor<T>::avg(int az) const {
  T aAvg = 0;
  int aSize = mXSize*mYSize;
  int aTemp = (az+1)*aSize;
  for (int i = az*aSize; i < aTemp; i++) 
    aAvg += mData[i];
  return aAvg/aSize;
}

// xSize
template <class T>
inline int CTensor<T>::xSize() const {
  return mXSize;
}

// ySize
template <class T>
inline int CTensor<T>::ySize() const {
  return mYSize;
}

// zSize
template <class T>
inline int CTensor<T>::zSize() const {
  return mZSize;
}

// size
template <class T>
inline int CTensor<T>::size() const {
  return mXSize*mYSize*mZSize;
}

// getMatrix
template <class T>
CMatrix<T> CTensor<T>::getMatrix(const int az) const {
  CMatrix<T> aTemp(mXSize,mYSize);
  int aMatrixSize = mXSize*mYSize;
  int aOffset = az*aMatrixSize;
  for (int i = 0; i < aMatrixSize; i++)
    aTemp.data()[i] = mData[i+aOffset];
  return aTemp;
}

// getMatrix
template <class T>
void CTensor<T>::getMatrix(CMatrix<T>& aMatrix, const int az) const {
  if (aMatrix.xSize() != mXSize || aMatrix.ySize() != mYSize)
    throw ETensorIncompatibleSize(aMatrix.xSize(),aMatrix.ySize(),mXSize,mYSize);
  int aMatrixSize = mXSize*mYSize;
  int aOffset = az*aMatrixSize;
  for (int i = 0; i < aMatrixSize; i++)
    aMatrix.data()[i] = mData[i+aOffset];
}

// putMatrix
template <class T>
void CTensor<T>::putMatrix(CMatrix<T>& aMatrix, const int az) {
  if (aMatrix.xSize() != mXSize || aMatrix.ySize() != mYSize)
    throw ETensorIncompatibleSize(aMatrix.xSize(),aMatrix.ySize(),mXSize,mYSize);
  int aMatrixSize = mXSize*mYSize;
  int aOffset = az*aMatrixSize;
  for (int i = 0; i < aMatrixSize; i++)
    mData[i+aOffset] = aMatrix.data()[i];
}

// data()
template <class T>
inline T* CTensor<T>::data() const {
  return mData;
}

// N O N - M E M B E R  F U N C T I O N S --------------------------------------

// operator <<
template <class T>
std::ostream& operator<<(std::ostream& aStream, const CTensor<T>& aTensor) {
  for (int z = 0; z < aTensor.zSize(); z++) {
    for (int y = 0; y < aTensor.ySize(); y++) {
      for (int x = 0; x < aTensor.xSize(); x++)
        aStream << aTensor(x,y,z) << ' ';
      aStream << std::endl;
    }
    aStream << std::endl;
  }
  return aStream;
}

#endif
