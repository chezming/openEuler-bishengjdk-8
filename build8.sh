umask 0022
chmod -R a+r bishengjdk-8
pushd bishengjdk-8
rm -rf build/
git pull origin
bash configure --with-extra-cflags='-fsigned-char -fno-aggressive-loop-optimizations -Wno-unused-parameter -fno-gnu-unique -fstack-protector-all' --with-extra-cxxflags='-fstack-protector-all' --with-extra-ldflags='-Wl,--build-id=none,-z,now ' --with-jvm-variants=server --enable-jfr --enable-kae=yes --enable-debug-symbols --disable-zip-debug-info --with-debug-level=release --with-boot-jdk=/home/huawei/bootjdk/bisheng-jdk1.8.0_302 --enable-unlimited-crypto --with-build-number=b12 --with-update-version=312 --with-milestone=fcs --with-vendor-url=https://gitee.com/openeuler/bishengjdk-8/ --with-vendor-bug-url=https://gitee.com/openeuler/bishengjdk-8/issues/ --with-vendor-vm-bug-url=https://gitee.com/openeuler/bishengjdk-8/issues/ --with-vendor-name=BiSheng
make images
pushd build/linux-aarch64-normal-server-release/images
find . -name "*.debuginfo" -exec rm -rf {} +
mv j2sdk-image bisheng-jdk1.8.0_312
tar czf bishengjdk8.tar.gz bisheng-jdk1.8.0_312
mv j2re-image bisheng-jre1.8.0_312
tar czf bishengjdk8-jre.tar.gz bisheng-jdk1.8.0_312
popd
mv build/linux-aarch64-normal-server-release/images/bishengjdk8*.tar.gz ..
popd
pushd gitee
git checkout obj8
mv ../bisheng*.tar.gz .
cp ../build8.sh .
git add . -A
git commit -a --amend -m "new jdk8"
git push origin -f
popd
