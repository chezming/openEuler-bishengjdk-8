umask 0022
chmod -R a+r bishengjdk-11
pushd bishengjdk-11
rm -rf build/
git pull origin
bash configure --with-version-pre= --with-debug-level=release --with-version-opt= --with-version-build=12 --with-vendor-url=https://gitee.com/openeuler/bishengjdk-11/ --with-vendor-bug-url=https://gitee.com/openeuler/bishengjdk-11/issues/ --with-vendor-vm-bug-url=https://gitee.com/openeuler/bishengjdk-11/issues/ --with-jvm-variants=server --with-boot-jdk=/home/huawei/bootjdk/bisheng-jdk-11.0.12 --with-vendor-name=BiSheng --with-vendor-version-string=BiSheng --with-native-debug-symbols=external --with-extra-cflags='-fstack-protector-all' --with-extra-cxxflags='-fstack-protector-all' --with-extra-ldflags='-Wl,-z,now,--build-id=none' --enable-kae
make images legacy-jre-image
pushd build/linux-aarch64-normal-server-release/images
find . -name "*.debuginfo" -exec rm -rf {} +
mv jdk bisheng-jdk-11.0.13
tar czf bishengjdk11.tar.gz bisheng-jdk-11.0.13
mv jre bisheng-jre-11.0.13
tar czf bishengjdk11-jre.tar.gz bisheng-jre-11.0.13
popd
mv build/linux-aarch64-normal-server-release/images/bishengjdk11*.tar.gz ..
popd
pushd gitee
git checkout obj11
mv ../bisheng*.tar.gz .
cp ../build11.sh .
git add . -A
git commit -a -m "new jdk11"
git push origin
popd
