@echo off
mkdir Dependency
mkdir Dependency\gtest
cd DependencyModules
cd googletest
cd msvc
IF EXIST gtest-md (
		cd gtest-md
		IF EXIST Release (
			cd Release
			    IF EXIST gtest.lib (
					copy "gtest.lib" "../../../../../Dependency/gtest.lib"
				) ELSE (
					ECHO can not find lib
				)
				cd ..
		) ELSE (
			IF EXIST Debug (
				cd Debug
				IF EXIST gtestd.lib (
					copy "gtestd.lib" "../../../../../Dependency/gtest.lib"
				) ELSE (
					ECHO can not find lib
				)
				cd ..
			) ELSE (
				ECHO can not find compiled google test framework
				ECHO you shuld compile google test(sln file is locate at DependencyModules\\googletest\\msvc)
			)
		)
		cd ..
		mkdir ..\..\..\Dependency\gtest\include
		xcopy "../include" "../../../Dependency/gtest/include" /E /Y
) ELSE (
	IF EXIST gtest (
		cd gtest
		IF EXIST Release (
			cd Release
			    IF EXIST gtest.lib (
					copy "gtest.lib" "../../../../../Dependency/gtest.lib"
				) ELSE (
					ECHO can not find lib
				)
				cd ..
		) ELSE (
			IF EXIST Debug (
				cd Debug
				IF EXIST gtestd.lib (
					copy "gtestd.lib" "../../../../../Dependency/gtest.lib"
				) ELSE (
					ECHO can not find lib
				)
				cd ..
			) ELSE (
				ECHO can not find compiled google test framework
				ECHO you shuld compile google test(sln file is locate at DependencyModules\\googletest\\msvc)
			)
		)
		cd ..
		mkdir ..\..\..\Dependency\gtest\include
		xcopy "../include" "../../../Dependency/gtest/include" /E /Y
	) ELSE (
		ECHO can not find compiled google test framework
		ECHO you shuld compile google test(sln file is locate at DependencyModules\googletest\msvc)
	)
)
cd ../../..