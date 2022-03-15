umask 0022
chmod -R a+r bishengjdk-17
popd
pushd bishengjdk-17
rm -rf build/
git pull origin
bash configure --with-version-pre= -with-debug-level=release --with-version-opt= --with-version-build=12 --with-vendor-url=https://gitee.com/openeuler/bishengjdk-17/ --with-vendor-bug-url=https://gitee.com/openeuler/bishengjdk-17/issues --with-vendor-vm-bug-url=https://gitee.com/openeuler/bishengjdk-17/issues --with-jvm-variants=server --with-boot-jdk=/home/huawei/bootjdk/jdk-17.0.1 --with-vendor-name=BiSheng --with-vendor-version-string=BiSheng --with-native-debug-symbols=external --with-extra-cflags='-fstack-protector-all' --with-extra-cxxflags='-fstack-protector-all' --with-extra-ldflags='-Wl,-z,now,--build-id=none'
make images legacy-jre-image
pushd build/linux-aarch64-server-release/images
find . -name "*.debuginfo" -exec rm -rf {} +
mv jdk bisheng-jdk-17.0.1
tar czf bishengjdk17.tar.gz bisheng-jdk-17.0.1
mv jre bisheng-jre-17.0.1
tar czf bishengjdk17-jre.tar.gz bisheng-jre-17.0.1
popd
mv build/linux-aarch64-server-release/images/bishengjdk17*.tar.gz ..
popd
pushd gitee
git checkout obj17
mv ../bisheng*.tar.gz .
cp ../build17.sh .
git add . -A
git commit -a --amend -m "new jdk17"
popd
