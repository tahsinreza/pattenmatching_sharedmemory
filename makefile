
all: .cmake-build-release-64 .cmake-build-release
	cmake --build .cmake-build-release-64/ --target all -- -j8 --no-print-directory
	cmake --build .cmake-build-release/ --target all -- -j8 --no-print-directory

$(MAKECMDGOALS): .cmake-build-release-64 .cmake-build-release
	cmake --build .cmake-build-release-64/ --target $(MAKECMDGOALS) -- -j8 --no-print-directory
	cmake --build .cmake-build-release/ --target $(MAKECMDGOALS) -- -j8 --no-print-directory

.cmake-build-release-64:
	-mkdir .cmake-build-release-64
	cd .cmake-build-release-64 && cmake -DCMAKE_BUILD_TYPE=Release -DEID=64 -DVID=64 ..

.cmake-build-release:
	-mkdir .cmake-build-release
	cd .cmake-build-release && cmake -DCMAKE_BUILD_TYPE=Release -DEID=32 -DVID=32 ..

clean:
	rm -rf build/

clean-cmake:
	rm -rf .cmake-build-*/

clean-all: clean clean-cmake
	
