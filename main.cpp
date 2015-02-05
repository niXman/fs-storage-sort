
#include "fs-storage-sort.hpp"

#include <cstring>
#include <iostream>
#include <random>

/***************************************************************************/

struct user {
	int age;
	char born[32];
	char name[32];
	char address[32];
}; // struct user

bool age_comparator(const user *l, const user *r) {
	return l->age > r->age;
}

bool born_comparator(const user *l, const user *r) {
	return std::strcmp(l->born, r->born) > 0;
}

bool name_comparator(const user *l, const user *r) {
	return std::strcmp(l->name, r->name) > 0;
}

bool address_comparator(const user *l, const user *r) {
	return std::strcmp(l->address, r->address) > 0;
}

/***************************************************************************/

template<typename C, typename T, typename F>
void user_sort_and_print(C *cmp, const std::size_t count, filesort::filesort<T> &fs, F f) {
	fs.sort(cmp);
	for ( std::size_t idx = 0; idx < count; ++idx ) {
		T u;
		fs.read(&u, idx);
		f(idx, u);
	}
}

void user_test(const char *fname, const std::size_t count) {
	filesort::filesort<user> fs(fname);

	std::random_device rd;
	for ( std::size_t idx = 0; idx < count; ++idx ) {
		user u;
		u.age = rd() % count;
		strncpy(u.born, std::to_string(rd()).c_str(), sizeof(u.born));
		strncpy(u.name, std::to_string(rd()).c_str(), sizeof(u.name));
		strncpy(u.address, std::to_string(rd()).c_str(), sizeof(u.address));

		fs.write(&u);
	}
	fs.flush();

	std::cout << "age:" << std::endl;
	user_sort_and_print(
		 age_comparator
		,count
		,fs
		,[](const std::size_t idx, const user &u) {
			std::cout << "idx=" << idx << ", age=" << u.age << std::endl;
		}
	);

	std::cout << "born:" << std::endl;
	user_sort_and_print(
		 born_comparator
		,count
		,fs
		,[](const std::size_t idx, const user &u) {
			std::cout << "idx=" << idx << ", born=" << u.born << std::endl;
		}
	);

	std::cout << "name:" << std::endl;
	user_sort_and_print(
		 name_comparator
		,count
		,fs
		,[](const std::size_t idx, const user &u) {
			std::cout << "idx=" << idx << ", name=" << u.name << std::endl;
		}
	);

	std::cout << "address:" << std::endl;
	user_sort_and_print(
		 address_comparator
		,count
		,fs
		,[](const std::size_t idx, const user &u) {
			std::cout << "idx=" << idx << ", address=" << u.address << std::endl;
		}
	);
}

/***************************************************************************/

int main() {

	user_test("test.dat", 1000);

	return 0;
}

/***************************************************************************/
