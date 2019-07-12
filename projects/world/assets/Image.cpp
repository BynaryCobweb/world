#include "Image.h"

#include <stdexcept>
#include <memory>

#include "world/math/MathsHelper.h"

namespace world {

inline u8 fromDouble(double f) {
    return clamp(static_cast<u8>(f * 255.0), (u8)0, (u8)255);
}

inline double toDouble(u8 u) { return u / 255.0; }
} // namespace world

#ifdef WORLD_BUILD_OPENCV_MODULES
#include "Interop.h"

#include <opencv/cv.h>

namespace world {

inline int getCVType(const ImageType &type) {
    switch (type) {
    case ImageType::RGB:
        return CV_8UC3;
    case ImageType::RGBA:
        return CV_8UC4;
    default:
        return CV_8UC1;
    }
}

inline ImageType getImageType(int cvType) {
    switch (cvType) {
    case CV_8UC1:
        return ImageType::GREYSCALE;
    case CV_8UC3:
        return ImageType::RGB;
    case CV_8UC4:
        return ImageType::RGBA;
    default:
        throw std::runtime_error("Unknown type");
    }
}

class PImage {
public:
    PImage(cv::Mat &&image) : _image(image) {}

    cv::Mat _image;
};

// ImageStream implementation

ImageStream::ImageStream(const world::Image &image) : _image(image) {}

int ImageStream::remaining() {
    auto &mat = _image._internal->_image;
    // size (in bytes) - position
    return static_cast<int>(mat.total() * mat.elemSize()) - _position;
}

int ImageStream::read(char *buffer, int count) {
    auto &mat = _image._internal->_image;
    int read = 0;
    const int s = static_cast<int>(mat.elemSize());

    while (count >= s) {
        u8 *data = mat.data + _position;

        switch (_image._type) {
        case ImageType::RGBA:
            // BGRA to ARGB
            buffer[read] = data[3];
            read++;
        case ImageType::RGB:
            buffer[read] = data[2];
            read++;
            buffer[read] = data[1];
            read++;
        case ImageType::GREYSCALE:
            buffer[read] = data[0];
            read++;
        }

        count -= s;
        _position += s;
    }

    return _position;
}

// GreyPixel

void GreyPixel::setLevel(u8 l) { _g = l; }

void GreyPixel::setLevelf(double l) { _g = fromDouble(l); }

u8 GreyPixel::getLevel() const { return _g; }

double GreyPixel::getLevelf(double l) { return toDouble(_g); }

// RGBPixel

u8 RGBPixel::getRed() const { return _b; }

u8 RGBPixel::getGreen() const { return _g; }

u8 RGBPixel::getBlue() const { return _r; }

double RGBPixel::getRedf() const { return toDouble(_b); }

double RGBPixel::getGreenf() const { return toDouble(_g); }

double RGBPixel::getBluef() const { return toDouble(_r); }

void RGBPixel::setRed(u8 r) { _b = r; }

void RGBPixel::setGreen(u8 g) { _g = g; }

void RGBPixel::setBlue(u8 b) { _r = b; }

void RGBPixel::setRedf(double r) { _b = fromDouble(r); }

void RGBPixel::setGreenf(double g) { _g = fromDouble(g); }

void RGBPixel::setBluef(double b) { _r = fromDouble(b); }

void RGBPixel::set(u8 r, u8 g, u8 b) {
    _b = r;
    _g = g;
    _r = b;
}

void RGBPixel::setf(double r, double g, double b) {
    _b = fromDouble(r);
    _g = fromDouble(g);
    _r = fromDouble(b);
}

// RGBAPixel

u8 RGBAPixel::getAlpha() const { return _a; }

double RGBAPixel::getAlphaf() const { return toDouble(_a); }

void RGBAPixel::setAlpha(u8 a) { _a = a; }

void RGBAPixel::setAlphaf(double a) { _a = fromDouble(a); }

void RGBAPixel::set(u8 r, u8 g, u8 b, u8 a) {
    RGBPixel::set(r, g, b);
    _a = a;
}

void RGBAPixel::setf(double r, double g, double b, double a) {
    RGBPixel::setf(r, g, b);
    _a = fromDouble(a);
}

// Image

Image::Image(int width, int height, const ImageType &type)
        : _internal(new PImage(cv::Mat(height, width, getCVType(type)))),
          _type(type) {}

Image::Image(const arma::Cube<double> &data) {
    _internal = new PImage(cv::Mat(armaToCV(data)));
    _type = getImageType(_internal->_image.type());
}

Image::Image(const arma::Mat<double> &data) {
    _internal = new PImage(cv::Mat(armaToCV(data)));
    _type = getImageType(_internal->_image.type());
}

Image::Image(const std::string &filename) {
    cv::Mat mat = cv::imread(filename);

    if (mat.data == nullptr) {
        throw std::ios_base::failure("File not found : " + filename);
    }

    _internal = new PImage(std::move(mat));
    _type = getImageType(_internal->_image.type());
}

Image::Image(const char *filename) : Image(std::string(filename)) {}

Image::Image(Image &&image) : _internal(image._internal), _type(image._type) {
    image._internal = nullptr;
}

Image::Image(const Image &image) {
    _internal = new PImage(cv::Mat(image._internal->_image));
    _type = image._type;
}

Image::~Image() {
    if (_internal != nullptr) // moved
        delete _internal;
}

ImageType Image::type() const { return _type; }

int Image::width() const { return _internal->_image.cols; }

int Image::height() const { return _internal->_image.rows; }

RGBAPixel &Image::rgba(int x, int y) {
    return _internal->_image.at<RGBAPixel>(y, x);
}

const RGBAPixel &Image::rgba(int x, int y) const {
    auto &img = _internal->_image;
    return _internal->_image.at<RGBAPixel>(y, x);
}

const RGBPixel &Image::rgb(int x, int y) const {
    auto &img = _internal->_image;
    return _internal->_image.at<RGBPixel>(y, x);
}

RGBPixel &Image::rgb(int x, int y) {
    return _internal->_image.at<RGBPixel>(y, x);
}

const GreyPixel &Image::grey(int x, int y) const {
    return _internal->_image.at<GreyPixel>(y, x);
}

GreyPixel &Image::grey(int x, int y) {
    auto &img = _internal->_image;
    return _internal->_image.at<GreyPixel>(y, x);
}

void Image::write(const std::string &file) const {
    cv::imwrite(file, _internal->_image);
}
} // namespace world

#else // WORLD_BUILD_OPENCV_MODULES

#include <libpng/png.h>

#include "world/core/StringOps.h"

namespace world {

u32 typeElemSize(const ImageType &type) {
    switch (type) {
    case ImageType::GREYSCALE:
        return 1;
    case ImageType::RGB:
        return 3;
    case ImageType::RGBA:
        return 4;
    }
}

class PImage {
public:
    PImage(u32 sizeX, u32 sizeY, u32 elemSize)
            : _data(new u8[sizeX * sizeY * elemSize]), _sizeX(sizeX),
              _sizeY(sizeY), _elemSize(elemSize) {}

    PImage(u8 *data, u32 sizeX, u32 sizeY, u32 elemSize)
            : _data(data), _sizeX(sizeX), _sizeY(sizeY), _elemSize(elemSize) {}

    ~PImage() { delete[] _data; };

    u8 *at(u32 x, u32 y) { return _data + (y * _sizeX + x) * _elemSize; }

    u32 total() { return _sizeX * _sizeY * _elemSize; }

    u8 *_data;
    u32 _sizeX;
    u32 _sizeY;
    u32 _elemSize;
};

// ImageStream implementation

ImageStream::ImageStream(const world::Image &image) : _image(image) {}

int ImageStream::remaining() {
    // size (in bytes) - position
    auto totalSize = _image._internal->total();
    return static_cast<int>(totalSize) - _position;
}

int ImageStream::read(char *buffer, int count) {
    auto &mat = *_image._internal;
    int read = 0;
    const int s = static_cast<int>(mat._elemSize);

    while (count >= s) {
        u8 *data = mat._data + _position;

        switch (_image._type) {
        case ImageType::RGBA:
            // RGBA to ARGB
            buffer[read] = data[3];
            read++;
        case ImageType::RGB:
            buffer[read] = data[0];
            read++;
            buffer[read] = data[1];
            read++;
            buffer[read] = data[2];
            read++;
            break;
        case ImageType::GREYSCALE:
            buffer[read] = data[0];
            read++;
        }

        count -= s;
        _position += s;
    }

    return _position;
}

// GreyPixel

void GreyPixel::setLevel(u8 l) { _g = l; }

void GreyPixel::setLevelf(double l) { _g = fromDouble(l); }

u8 GreyPixel::getLevel() const { return _g; }

double GreyPixel::getLevelf(double l) { return toDouble(_g); }

// RGBPixel

u8 RGBPixel::getRed() const { return _r; }

u8 RGBPixel::getGreen() const { return _g; }

u8 RGBPixel::getBlue() const { return _b; }

double RGBPixel::getRedf() const { return toDouble(_r); }

double RGBPixel::getGreenf() const { return toDouble(_g); }

double RGBPixel::getBluef() const { return toDouble(_b); }

void RGBPixel::setRed(u8 r) { _r = r; }

void RGBPixel::setGreen(u8 g) { _g = g; }

void RGBPixel::setBlue(u8 b) { _b = b; }

void RGBPixel::setRedf(double r) { _r = fromDouble(r); }

void RGBPixel::setGreenf(double g) { _g = fromDouble(g); }

void RGBPixel::setBluef(double b) { _b = fromDouble(b); }

void RGBPixel::set(u8 r, u8 g, u8 b) {
    _r = r;
    _g = g;
    _b = b;
}

void RGBPixel::setf(double r, double g, double b) {
    _r = fromDouble(r);
    _g = fromDouble(g);
    _b = fromDouble(b);
}

// RGBAPixel

u8 RGBAPixel::getAlpha() const { return _a; }

double RGBAPixel::getAlphaf() const { return toDouble(_a); }

void RGBAPixel::setAlpha(u8 a) { _a = a; }

void RGBAPixel::setAlphaf(double a) { _a = fromDouble(a); }

void RGBAPixel::set(u8 r, u8 g, u8 b, u8 a) {
    RGBPixel::set(r, g, b);
    _a = a;
}

void RGBAPixel::setf(double r, double g, double b, double a) {
    RGBPixel::setf(r, g, b);
    _a = fromDouble(a);
}

// Implémentation de Image

Image::Image(int width, int height, const ImageType &type)
        : _internal(new PImage(static_cast<u32>(width),
                               static_cast<u32>(height), typeElemSize(type))),
          _type(type) {}

Image::Image(const arma::Cube<double> &data) : _type(ImageType::RGB) {

    _internal = new PImage(static_cast<u32>(data.n_rows),
                           static_cast<u32>(data.n_cols), 3);

    auto width = _internal->_sizeX;
    auto height = _internal->_sizeY;

    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width; ++x) {
            auto ptr = _internal->at(x, y);
            ptr[0] = fromDouble(data(x, y, 0));
            ptr[1] = fromDouble(data(x, y, 1));
            ptr[2] = fromDouble(data(x, y, 2));
        }
    }
}

Image::Image(const arma::Mat<double> &data) : _type(ImageType::GREYSCALE) {

    _internal = new PImage(static_cast<u32>(data.n_rows),
                           static_cast<u32>(data.n_cols), 1);

    auto width = _internal->_sizeX;
    auto height = _internal->_sizeY;

    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width; ++x) {
            *_internal->at(x, y) = fromDouble(data(x, y));
        }
    }
}

Image::Image(const std::string &filename) {
    throw std::runtime_error("Not supported");
}

Image::Image(const char *filename) : Image(std::string(filename)) {}

Image::Image(Image &&image) : _internal(image._internal), _type(image._type) {
    image._internal = nullptr;
}

Image::Image(const Image &image) {
    auto pother = image._internal;
    _internal = new PImage(pother->_sizeX, pother->_sizeY, pother->_elemSize);
    memcpy(_internal->_data, pother->_data, pother->total());
    _type = image._type;
}

Image::~Image() {
    if (_internal != nullptr) // moved
        delete _internal;
}

Image &Image::operator=(const Image &img) {
    operator=(Image(img));
    return *this;
}

Image &Image::operator=(Image &&img) {
    _internal = img._internal;
    _type = img._type;
    img._internal = nullptr;
    return *this;
}

ImageType Image::type() const { return _type; }

int Image::elemSize() const { return _internal->_elemSize; }

int Image::width() const { return _internal->_sizeX; }

int Image::height() const { return _internal->_sizeY; }

int Image::size() const { return _internal->total(); }

RGBAPixel &Image::rgba(int x, int y) {
    return *reinterpret_cast<RGBAPixel *>(_internal->at(x, y));
}

const RGBAPixel &Image::rgba(int x, int y) const {
    return *reinterpret_cast<RGBAPixel *>(_internal->at(x, y));
}

RGBPixel &Image::rgb(int x, int y) {
    return *reinterpret_cast<RGBPixel *>(_internal->at(x, y));
}

const RGBPixel &Image::rgb(int x, int y) const {
    return *reinterpret_cast<RGBPixel *>(_internal->at(x, y));
}

GreyPixel &Image::grey(int x, int y) {
    return *reinterpret_cast<GreyPixel *>(_internal->at(x, y));
}

const GreyPixel &Image::grey(int x, int y) const {
    return *reinterpret_cast<GreyPixel *>(_internal->at(x, y));
}

void Image::setf(int x, int y, const float *values) {
    u8 *target = _internal->at(x, y);

    for (u32 i = 0; i < _internal->_elemSize; ++i) {
        target[i] = fromDouble(values[i]);
    }
}

void Image::getf(int x, int y, float *values) const {
    const u8 *src = _internal->at(x, y);

    for (u32 i = 0; i < _internal->_elemSize; ++i) {
        values[i] = src[i];
    }
}

// Check endianness (method from opencv/util.hpp)
bool isBigEndian(void) {
    return (((const int *)"\0\x1\x2\x3\x4\x5\x6\x7")[0] & 255) != 0;
}

// inspired by the example here : http://zarb.org/~gc/html/libpng.html
void Image::write(const std::string &path) const {
    // check path ends with .png
    if (!endsWith(path, ".png")) {
        throw std::runtime_error(std::string("Unsupported format for file ") +
                                 path + ". We only support png at the moment.");
    }

    FILE *file = fopen(path.c_str(), "wb");

    if (!file) {
        perror("Open file failed ");
        throw std::ios_base::failure("Can't open " + path);
    }

    png_structp png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, file);

    if (setjmp(png_jmpbuf(png_ptr))) {
        // TODO error handling
    }

    int colortype;
    switch (_type) {
    case ImageType::GREYSCALE:
        colortype = PNG_COLOR_TYPE_GRAY;
        break;
    case ImageType::RGB:
        colortype = PNG_COLOR_TYPE_RGB;
        break;
    case ImageType::RGBA:
        colortype = PNG_COLOR_TYPE_RGB_ALPHA;
        break;
    }

    png_set_IHDR(png_ptr, info_ptr, _internal->_sizeX, _internal->_sizeY, 8,
                 colortype, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    // write image
    png_bytep *rowptrs = new png_bytep[_internal->_sizeY];

    for (u32 y = 0; y < _internal->_sizeY; y++) {
        rowptrs[y] = _internal->at(0, y);
    }

    if (!isBigEndian()) {
        png_set_swap(png_ptr);
    }
    png_write_image(png_ptr, rowptrs);

    // write end and close file
    // We pass NULL as second parameter to avoid writing comments and metadata a
    // second time
    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(file);

    // free memory
    delete[] rowptrs;
}
} // namespace world

#endif // WORLD_BUILD_OPENCV_MODULES