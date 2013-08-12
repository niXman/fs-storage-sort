
#ifndef _file_sort_hpp
#define _file_sort_hpp

#include <cstdint>
#include <cassert>
#include <cstdio>
#include <string>
#include <memory>

namespace filesort {

/***************************************************************************/

template<typename T>
struct filesort {
  filesort(const std::string &fname) {
		file = std::fopen(fname.c_str(), "w+b");
		assert(file);
	}
	~filesort()
	{ fclose(file); }

	void write(const T *item, const std::uint64_t pos = -1) {
		if ( pos == -1ul ) {
			assert(std::fwrite(item, 1, sizeof(T), file) == sizeof(T));
		} else {
			seek(pos);
			write(item, -1);
		}
	}

	void read(T *item, const std::uint64_t pos = -1) {
		if ( pos == -1ul ) {
			assert(std::fread(item, 1, sizeof(T), file) == sizeof(T));
		} else {
			seek(pos);
			read(item, -1);
		}
	}

	std::uint64_t fsize() const {
		std::fseek(file, 0, SEEK_END);
		return std::ftell(file);
	}
	std::uint64_t items() const {
		return fsize()/sizeof(T);
	}
	void seek(const std::uint64_t idx) {
		std::fseek(file, idx*sizeof(T), SEEK_SET);
	}
	std::uint64_t tell() const {
		return std::ftell(file)/sizeof(T);
	}
	void flush() { std::fflush(file); }

	void sort(bool(*cmp)(const T*, const T*)) {
		sort(cmp, -1, -1);
	}
	void sort(bool(*cmp)(const T*, const T*), std::uint64_t left, std::uint64_t right) {
		std::uint64_t base, opposite;

		std::unique_ptr<T> tmp1(new T());
		std::unique_ptr<T> tmp2(new T());

		if ( left == -1ul || right == -1ul ) {
			left = 0;
			right = items();
			assert(right > 0);
			right -= 1;
		}

		if ( (right-left) < 2 ) return;

		base = left;
		opposite = right;

		read(tmp1.get(), base);

		while ( base != opposite ) {
			read(tmp2.get(), opposite);
			if ( cmp(tmp1.get(), tmp2.get()) ^ (base > opposite) ) {
				write(tmp2.get(), base);
				write(tmp1.get(), opposite);

				const std::uint64_t p = base;
				base = opposite;

				read(tmp1.get(), base);
				opposite = (p < opposite) ? p+1 : p-1;
			} else {
				opposite = (base < opposite) ? opposite-1 : opposite+1;
			}
		}

		if ( left < base-1 ) {
			if ( base == 0 ) return;
			sort(cmp, left, base-1);
		}
		if ( base+1 < right ) {
			if ( (base + 1) == right ) return;
			sort(cmp, base+1, right);
		}
	}

private:
	FILE *file;

private:
	filesort(const filesort &) = delete;
};

/***************************************************************************/

} // ns filesort

#endif // _file_sort_hpp
