#!/bin/bash

ITCHIO_PROJECT_NAME=tyrebytes/asteroids
ITCHIO_RELEASE_CHANNEL=alpha
TBDOTCOM_SERVERPATH=timbeaudet@timbeaudet.com
TBDOTCOM_BUILD_LOCATION=tyrebytes.com/games/asteroids/downloads/
BANDWIDTH_LIMIT_KBPS=1400

SHOULD_LIMIT_BANDWIDTH_HACK=false
BANDWIDTH_LIMIT_INTERFACE=enp2s0

# This came from good old fluffy.
if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
else
    CORES=$(getconf _NPROCESSORS_ONLN)
fi

FLAG_HELP=false
FLAG_CLEAN=true
FLAG_BUILD_CONFIG=public
FLAG_MAKE_JOBS=$CORES
FLAG_LINUX=false
FLAG_MACOS=false
FLAG_WINDOWS=false
FLAG_OFFLINE=false
FLAG_SKIP_ITCH=false
FLAG_SKIP_STEAM=false
FLAG_SKIP_TYREBYTES=false
BUILD_ALL_PLATFORMS=true

POSITIONAL=()
while [[ $# -gt 0 ]]; do
	case "$1" in
		--help ) FLAG_HELP=true; shift ;;
		--clean ) FLAG_CLEAN=true; shift ;;
		--dirty ) FLAG_CLEAN=false; shift ;;
		-d | --debug ) FLAG_BUILD_CONFIG=debug; shift ;;
		-r | --release ) FLAG_BUILD_CONFIG=release; shift ;;
		-p | --public ) FLAG_BUILD_CONFIG=public; shift ;;
		-j ) FLAG_MAKE_JOBS="$2"; shift 2 ;;
		--linux ) FLAG_LINUX=true; BUILD_ALL_PLATFORMS=false; shift ;;
		--macos ) FLAG_MACOS=true; BUILD_ALL_PLATFORMS=false; shift ;;
		--windows ) FLAG_WINDOWS=true; BUILD_ALL_PLATFORMS=false; shift ;;
		--offline ) FLAG_OFFLINE=true; shift ;;
		--limit ) SHOULD_LIMIT_BANDWIDTH_HACK=true; BANDWIDTH_LIMIT_KBPS="$2"; shift 2 ;;
		--skip-itch ) FLAG_SKIP_ITCH=true; shift ;;
		--itch ) FLAG_SKIP_ITCH=false; shift ;;
		--skip-steam ) FLAG_SKIP_STEAM=true; shift ;;
		--steam ) FLAG_SKIP_STEAM=false; shift ;;
		--skip-tyre ) FLAG_SKIP_TYREBYTES=true; shift ;;
		-tyre ) FLAG_SKIP_TYREBYTES=false; shift ;;
		-* ) echo "Unknown parameter \"$1\""; exit 2 ;;
		* ) POSITIONAL+=("$1"); shift ;;
	esac
done

DIR=`dirname "$(readlink -f "$0")"`
kLinuxPlatform="Linux"
currentPlatform=`uname`

# echo "${POSITIONAL[*]}"

# 2024-09-03: No longer specifying the version through build_and_distribute script calls, use the version.txt file and
#   keep that committed to source control as the builds are created.
#BUILD_VERSION=${POSITIONAL[0]}
BUILD_VERSION=$(cat version.txt)
if [ -z ${BUILD_VERSION} ]; then
	BUILD_VERSION=0.0.0
fi

print_usage() {
	cat <<EOF
Usage:
  $0 <version string> [flags]

Flags:
  --help        show this help
  --dirty       skip the clean build
  --offline     perform a dry run without uploading
  -d, --debug   build in debug mode
  -r, --release build in release mode
  -p, --public  build in public mode (default if -d, -r and -p not specified)
  -j [N]        build with N concurrent jobs
  --skip-itch   skips the itch.io deployment (default is project dependent)
  --itch        also deploy to itch.io (default is project dependent)
  --limit [N]   limit upload speed to N kb/s

If none of these flags are specified, all of them will apply:
  --linux       build for Linux
  --macos       build for MacOS
  --windows     distribute for Windows
EOF
}

build_mac() (
	set -e
	echo "--> Building for MacOS"

	if [ $kLinuxPlatform == $currentPlatform ]; then
		echo "Not attempting to use crossmac, make sure to run macOS build and put in rhino://game_builds"
		# echo "Attempting to use crossmac to build macOS from Linux"

		# #	For some reason, the following does not work, it seems to give fatal errors string not found
		# #	Much like the crossmac can't find standard headers or something?
		# #	[ $FLAG_BUILD_CONFIG == "debug" ] && DEBUG="--debug" || DEBUG=""
		# #	[ $FLAG_CLEAN == true ] && CLEAN="--clean" || CLEAN=""
		# #	$DIR/make_project.sh --macos --build-version $BUILD_VERSION -j $FLAG_MAKE_JOBS $DEBUG $CLEAN

		# if [[ $FLAG_CLEAN = true ]] || [[ ! -f $DIR/macos/asteroids.make ]]; then
		# 	if [[ $FLAG_CLEAN = true ]]; then
		# 		premake5 --file=$DIR/asteroids.lua --os=macosx --build-version=$BUILD_VERSION clean
		# 	fi
		# 	premake5 --file=$DIR/asteroids.lua --os=macosx --build-version=$BUILD_VERSION gmake
		# fi

		# sed -i 's/\(CC\|CXX\|AR\) = \(clang\|clang++\|ar\)/\1 = x86_64-apple-darwin15-\2/' $DIR/macos/asteroids.make

		# make -C $DIR/macos config=$FLAG_BUILD_CONFIG -j $FLAG_MAKE_JOBS
		# x86_64-apple-darwin15-strip $DIR/macos/$FLAG_BUILD_CONFIG/asteroids

		# echo "--> Moving files to distribution/"
		# mkdir -p $DIR/../distribution/macos/Asteroids.app/Contents/MacOS/
		# rsync -avhiP $DIR/macos/$FLAG_BUILD_CONFIG/asteroids $DIR/../distribution/macos/Asteroids.app/Contents/MacOS/Asteroids
		# rsync -avhiP $DIR/../run/data $DIR/../distribution/macos/Asteroids.app/Contents/Resources/ --exclude ".git"
	else
		echo BuildAndDistribute: Building a clean public build of Asteroids v${BUILD_VERSION}
		source make_project.sh --build-version ${BUILD_VERSION} --public --clean

		# 2025-01-12: There is VERY LIKELY always a false-positive here as I do not believe make_project.bat or sh returns a
		#   non-zero value when the build fails. So that is probably desired... So in here we are not testing the error level
		#   since it didn't exist yet. Still probably want to someday.

		for GAME_EXECUTABLE in asteroids asteroids_steam asteroids_demo asteroids_demo_steam; do
			DEMO_SUFFIX=
			if [[ "$GAME_EXECUTABLE" == *demo* ]]; then
				DEMO_SUFFIX=_demo
			fi

			echo BuildAndDistribute: Creating a packaged application/zip to upload on itch.io and tyrebytes.com/games

			RELEASE_APP_DIRECTORY="../distribution/macos/${GAME_EXECUTABLE}_${BUILD_VERSION}_macos.app"

			#If the macOS application package already exists, first delete it before recreating.
			if [ -d "${RELEASE_APP_DIRECTORY}" ]; then
				rm -r "${RELEASE_APP_DIRECTORY}"
			fi
			if [[ -f "${RELEASE_APP_DIRECTORY}.zip" ]]; then
				rm -f "${RELEASE_APP_DIRECTORY}.zip"
			fi

			#Make the directory structure of the macOS application package, then copy executable and data into package.
			# 2025-01-16: We should probably copy over the release_notes.md and more importantly the libsteam_api.dylib needs
			#   to go somewhere. Nobody is testing macos yet, so, lets deal with that 'later'.
			echo "BuildAndDistribute: Package will be placed at: $RELEASE_APP_DIRECTORY"
			mkdir -p "${RELEASE_APP_DIRECTORY}/Contents/Resources/data"
			mkdir -p "${RELEASE_APP_DIRECTORY}/Contents/MacOS"
			cp "macos/public/${GAME_EXECUTABLE}.app/Contents/MacOS/${GAME_EXECUTABLE}" "${RELEASE_APP_DIRECTORY}/Contents/MacOS/${GAME_EXECUTABLE}"
			cp "../run/libsteam_api.dylib" "${RELEASE_APP_DIRECTORY}/Contents/MacOS/"
			cp -r "../run/data/" "${RELEASE_APP_DIRECTORY}/Contents/Resources/data/"

			# Create compressed ZIP archive of the application package.
			echo "BuildAndDistribute: Compressing to zip..."

			ZIPPED_GAME_APP="../distribution/macos/${GAME_EXECUTABLE}_${BUILD_VERSION}_macos.app.zip"
			RHINO_GAME_BUILDS=timbeaudet@192.168.1.100:/mnt/z/game_builds/asteroids${DEMO_SUFFIX}/${BUILD_VERSION}/

			if [[ -f "${ZIPPED_GAME_APP}" ]]; then
				rm "${ZIPPED_GAME_APP}"
			fi

			# 2025-11-11: This push/pop is actually important, as the zip must be in the current directory or
			#   it fills in the wrong path information...
			pushd "${RELEASE_APP_DIRECTORY}"
			zip -r "../${GAME_EXECUTABLE}_${BUILD_VERSION}_macos.app.zip" ./
			popd

			echo "BuildAndDistribute: Build was a success, compressed to zip, now moving to rhino for further distribution."

			mkdir -p ${RHINO_GAME_BUILDS}
			rsync -avhiP ${RELEASE_APP_DIRECTORY}.zip ${RHINO_GAME_BUILDS}/${GAME_EXECUTABLE}_${BUILD_VERSION}_macos.app.zip
			echo "Copied to rhino/shelter/development/game_builds/ - Upload using build_and_distribute.sh from rhino."
		done

		# echo "--> Moving files to distribution/"
		# mkdir -p $DIR/../distribution/macos/Asteroids.app/

		# rsync -avhiP $DIR/macos/$FLAG_BUILD_CONFIG/asteroids.app/ $DIR/../distribution/macos/Asteroids.app/
		# rsync -avhiP $DIR/../run/data $DIR/../distribution/macos/Asteroids.app/Contents/Resources/ --exclude ".git"
		# mv $DIR/../distribution/macos/Asteroids.app/Contents/MacOS/asteroids $DIR/../distribution/macos/Asteroids.app/Contents/MacOS/Asteroids

		# zip -r $DIR/../distribution/macos/asteroids_${BUILD_VERSION}_macos.zip $DIR/../distribution/macos/Asteroids.app

		# rsync -avhiP "$DIR/../distribution/macos/asteroids_${BUILD_VERSION}_macos.zip" timbeaudet@192.168.1.100:z/game_builds/asteroids/$BUILD_VERSION/
	fi
)

build_linux() (
	set -e
	echo "--> Building for Linux"

	# [ $FLAG_CLEAN == true ] && CLEAN="--clean" || CLEAN=""
	# $DIR/make_project.sh --linux --build-version $BUILD_VERSION -j $FLAG_MAKE_JOBS --$FLAG_BUILD_CONFIG $CLEAN
	# strip $DIR/linux/$FLAG_BUILD_CONFIG/asteroids

	# echo "--> Moving files to distribution/"
	# mkdir -p $DIR/../distribution/linux/asteroids/
	# rsync -avhiP $DIR/linux/$FLAG_BUILD_CONFIG/asteroids $DIR/../distribution/linux/asteroids/
	# rsync -avhiP $DIR/../run/data $DIR/../run/readme.txt $DIR/../distribution/linux/asteroids/ --exclude ".git"

	$DIR/make_project.sh --build-version $BUILD_VERSION --linux --public --clean

	for GAME_EXECUTABLE in asteroids asteroids_steam asteroids_demo asteroids_demo_steam; do
		DEMO_SUFFIX=
		if [[ "$GAME_EXECUTABLE" == *demo* ]]; then
			DEMO_SUFFIX=_demo
		fi

		echo BuildAndDistribute: Creating a packaged zip to upload on itch.io and tyrebytes.com/games
		RELEASE_APP_DIRECTORY="../distribution/linux/${GAME_EXECUTABLE}_${BUILD_VERSION}_linux"

		echo "--> Moving files to distribution/"
		mkdir -p $RELEASE_APP_DIRECTORY

		rsync -avhiP $DIR/linux/public/${GAME_EXECUTABLE} ${RELEASE_APP_DIRECTORY}
		rsync -avhiP $DIR/../run/{data,readme.txt,release_notes.md,libsteam_api.so} ${RELEASE_APP_DIRECTORY} --exclude ".git"
		strip ${RELEASE_APP_DIRECTORY}/${GAME_EXECUTABLE}

		# Create compressed ZIP archive of the application package.
		# It has seemed that we need to use ./ for zip, I don't quite understand why other than "it is relative"...
		pushd "${RELEASE_APP_DIRECTORY}"
		zip -r "../${GAME_EXECUTABLE}_${BUILD_VERSION}_linux.zip" ./
		popd

		tar czf "${RELEASE_APP_DIRECTORY}.tar.gz" "$RELEASE_APP_DIRECTORY"

		echo "BuildAndDistribute: Build was a success, compressed to zip, now moving to rhino for further distribution."
		rsync -avhiP ${RELEASE_APP_DIRECTORY}.zip /mnt/z/game_builds/asteroids${DEMO_SUFFIX}/${BUILD_VERSION}/${GAME_EXECUTABLE}_${BUILD_VERSION}_linux.zip
		rsync -avhiP ${RELEASE_APP_DIRECTORY}.tar.gz /mnt/z/game_builds/asteroids${DEMO_SUFFIX}/${BUILD_VERSION}/${GAME_EXECUTABLE}_${BUILD_VERSION}_linux.tar.gz
		echo "Copied to rhino/shelter/development/game_builds/ - Upload using build_and_distribute.sh from rhino."
	done
)

###
### stuff starts here!
###

if [[ $BUILD_VERSION = "" ]] || [[ $FLAG_HELP = true ]]; then
	print_usage
	exit 1
fi

echo "--> Creating builds for version \"$BUILD_VERSION\""

if [[ $BUILD_ALL_PLATFORMS = true ]] || [[ $FLAG_MACOS = true ]]; then
	if [ $kLinuxPlatform != $currentPlatform ]; then
		build_mac
		BUILD_STATUS_MAC=$?
		echo $BUILD_STATUS_MAC
	fi
fi

# The rest of this only happens on Linux, (rhino)
if [ $kLinuxPlatform == $currentPlatform ]; then

	if [[ $BUILD_ALL_PLATFORMS = true ]] || [[ $FLAG_LINUX = true ]]; then
		build_linux
		BUILD_STATUS_LINUX=$?
		echo $BUILD_STATUS_LINUX
	fi

	GAME_BUILDS_DIR=/mnt/z/game_builds/
	if [[ $BUILD_ALL_PLATFORMS = true ]] || [[ $FLAG_WINDOWS = true ]]; then
		BUILD_STATUS_WINDOWS=1
		if [[ -f "${GAME_BUILDS_DIR}/asteroids/${BUILD_VERSION}/asteroids_${BUILD_VERSION}_windows.zip" ]]; then
			BUILD_STATUS_WINDOWS=0
		fi
		echo $BUILD_STATUS_WINDOWS
	fi

	if [[ $BUILD_ALL_PLATFORMS = true ]] || [[ $FLAG_MACOS = true ]]; then
		BUILD_STATUS_MAC=1
		if [[ -f "${GAME_BUILDS_DIR}/asteroids/${BUILD_VERSION}/asteroids_${BUILD_VERSION}_macos.app.zip" ]]; then
			BUILD_STATUS_MAC=0
		fi
		echo $BUILD_STATUS_MAC
	fi

	if [[ $FLAG_OFFLINE == true ]]; then
		echo "Not uploading because --offline was specified."
	else
		echo "--> Distributing packages"

		if [[ $SHOULD_LIMIT_BANDWIDTH_HACK == true ]]; then
			echo "Attempting to limit bandwidth, needs sudo password."
			sudo tc qdisc add dev $BANDWIDTH_LIMIT_INTERFACE handle 1: root htb default 11
			sudo tc class add dev $BANDWIDTH_LIMIT_INTERFACE parent 1: classid 1:1 htb rate 1000Mbps
			sudo tc class add dev $BANDWIDTH_LIMIT_INTERFACE parent 1:1 classid 1:11 htb rate ${BANDWIDTH_LIMIT_KBPS}kbit
		fi

		if [[ $BUILD_STATUS_WINDOWS == 0 ]]; then
			echo "--> Distributing Windows package"

			ZIP_FILENAME=${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/asteroids_${BUILD_VERSION}_windows.zip
			#INSTALLER_FILENAME=${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/AsteroidsInstaller_${BUILD_VERSION}.exe

			if [[ $FLAG_SKIP_ITCH == false ]]; then
				[ $FLAG_BUILD_CONFIG == "debug" ] && DEBUG="-debug" || DEBUG=""
				butler push --userversion $BUILD_VERSION $ZIP_FILENAME ${ITCHIO_PROJECT_NAME}:windows${DEBUG}-${ITCHIO_RELEASE_CHANNEL}
			fi

			if [[ $FLAG_SKIP_TYREBYTES == false ]]; then
				echo "-->   Uploading asteroids_${BUILD_VERSION}_windows.zip to ${TBDOTCOM_SERVERPATH}"
				# --whole-file may be faster dispite uploading more bytes in total, may actually speed up w/ slow webhost.
				rsync --archive --verbose --human-readable --itemize-changes --partial --whole-file --progress --bwlimit=$BANDWIDTH_LIMIT_KBPS $ZIP_FILENAME $TBDOTCOM_SERVERPATH:$TBDOTCOM_BUILD_LOCATION
				if [[ -f "$INSTALLER_FILENAME" ]]; then
					echo "-->   Uploading ${INSTALLER_FILENAME} to ${TBDOTCOM_SERVERPATH}"
					rsync --archive --verbose --human-readable --itemize-changes --partial --whole-file --progress --bwlimit=$BANDWIDTH_LIMIT_KBPS $INSTALLER_FILENAME $TBDOTCOM_SERVERPATH:$TBDOTCOM_BUILD_LOCATION
				fi
			fi
		fi

		if [[ $BUILD_STATUS_LINUX == 0 ]]; then
			echo "--> Distributing Linux package"

			TAR_FILENAME=asteroids_${BUILD_VERSION}_linux.tar.gz

			(cd $DIR/../distribution/linux && tar czf ../$TAR_FILENAME asteroids)

			if [[ $FLAG_SKIP_ITCH == false ]]; then
				[ $FLAG_BUILD_CONFIG == "debug" ] && DEBUG="-debug" || DEBUG=""
				butler push --userversion $BUILD_VERSION $DIR/../distribution/$TAR_FILENAME ${ITCHIO_PROJECT_NAME}:linux64${DEBUG}-${ITCHIO_RELEASE_CHANNEL}
			fi

			if [[ $FLAG_SKIP_TYREBYTES == false ]]; then
				echo "-->   Uploading ${TAR_FILENAME} to {$TBDOTCOM_SERVERPATH}"
				# --whole-file may be faster dispite uploading more bytes in total, may actually speed up w/ slow webhost.
				rsync --archive --verbose --human-readable --itemize-changes --partial --progress --whole-file --bwlimit=$BANDWIDTH_LIMIT_KBPS $DIR/../distribution/$TAR_FILENAME $TBDOTCOM_SERVERPATH:$TBDOTCOM_BUILD_LOCATION
			fi

			# 2025-11-11: Don't need to copy this since it happens now in an offline distribute too.
			# rsync --archive --verbose --human-readable --itemize-changes --partial --progress $DIR/../distribution/$TAR_FILENAME ${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/$TAR_FILENAME
		fi

		if [[ $BUILD_STATUS_MAC == 0 ]]; then
			echo "--> Distributing MacOS package"

			# if [[ crossmac ]]; then
			# 	(cd $DIR/../distribution/macos && zip -r9 ../asteroids_${BUILD_VERSION}_macos.zip .)

			# 	if [[ $FLAG_SKIP_ITCH == false ]]; then
			# 		[ $FLAG_BUILD_CONFIG == "debug" ] && DEBUG="-debug" || DEBUG=""
			# 		# Tell Butler to use the .app directory directly, because it will create a
			# 		# zip itself and also a patch for the Itch desktop app users
			# 		# (I don't know if patch stuff works if you upload your own .zip)
			# 		butler push --userversion $BUILD_VERSION $DIR/../distribution/macos/ ${ITCHIO_PROJECT_NAME}:macos64${DEBUG}-${ITCHIO_RELEASE_CHANNEL}
			# 	fi

			# 	rsync --archive --verbose --human-readable --itemize-changes --partial --progress --bwlimit=$BANDWIDTH_LIMIT_KBPS $DIR/../distribution/asteroids_${BUILD_VERSION}_macos.zip $TBDOTCOM_SERVERPATH:$TBDOTCOM_BUILD_LOCATION
			# 	rsync --archive --verbose --human-readable --itemize-changes --partial --progress $DIR/../distribution/$MACOS_ZIP_FILENAME ${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/$MACOS_ZIP_FILENAME
			# else

				ZIP_FILENAME=${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/asteroids_${BUILD_VERSION}_macos.app.zip
				#INSTALLER_FILENAME=${GAME_BUILDS_DIR}asteroids/${BUILD_VERSION}/AsteroidsInstaller_${BUILD_VERSION}.exe

				if [[ $FLAG_SKIP_ITCH == false ]]; then
					[ $FLAG_BUILD_CONFIG == "debug" ] && DEBUG="-debug" || DEBUG=""
					butler push --userversion $BUILD_VERSION $ZIP_FILENAME ${ITCHIO_PROJECT_NAME}:macos${DEBUG}-${ITCHIO_RELEASE_CHANNEL}
				fi

				if [[ $FLAG_SKIP_TYREBYTES == false ]]; then
					echo "-->   Uploading ${ZIP_FILENAME} to ${TBDOTCOM_SERVERPATH}"
					# --whole-file may be faster dispite uploading more bytes in total, may actually speed up w/ slow webhost.
					rsync --archive --verbose --human-readable --itemize-changes --whole-file \
						--partial --progress --bwlimit=$BANDWIDTH_LIMIT_KBPS $ZIP_FILENAME $TBDOTCOM_SERVERPATH:$TBDOTCOM_BUILD_LOCATION
				fi
			#fi
		fi

		if [[ $FLAG_SKIP_STEAM == false ]]; then
			if [[ $BUILD_STATUS_WINDOWS == 0 && $BUILD_STATUS_MAC == 0 && $BUILD_STATUS_LINUX == 0 ]]; then
				(cd "./steam" && source "./distribute_to_steam.sh")
			else
				echo "ERROR: NOT DISTRIBUTING TO STEAM; one or more builds not ready. All should be 0:"
				echo "Windows: ${BUILD_STATUS_WINDOWS}"
				echo "macOS: ${BUILD_STATUS_MAC}"
				echo "Linux: ${BUILD_STATUS_LINUX}"
			fi
		fi

		if [[ $SHOULD_LIMIT_BANDWIDTH_HACK == true ]]; then
			echo "Attempting to stop bandwidth limit, needs sudo password."
			sudo tc qdisc del dev $BANDWIDTH_LIMIT_INTERFACE root
		fi
	fi

	echo "Build and distribute done!"
fi
