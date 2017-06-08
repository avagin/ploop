set -x -e

export LD_LIBRARY_PATH=./lib/

TEST_DIR=/vz/vstorage/test
export PLOOP_DELTA_DIR=$TEST_DIR/delta


mkdir $TEST_DIR
mkdir $PLOOP_DELTA_DIR
mkdir $TEST_DIR/ploop1
mkdir $TEST_DIR/ploop1-mnt
mkdir $TEST_DIR/ploop2-mnt
./tools/ploop init -t ext4 -s 16G $TEST_DIR/ploop1/base.{`uuid`}
ploop mount $TEST_DIR/ploop1/DiskDescriptor.xml -m $TEST_DIR/ploop1-mnt

touch $TEST_DIR/ploop1-mnt/test-file
ls -l $TEST_DIR/ploop1-mnt

# Create snapshot
cp -r $TEST_DIR/ploop1 $TEST_DIR/snap1
ploop snapshot -F $TEST_DIR/ploop1/DiskDescriptor.xml

# Create a second ploop
cp -r $TEST_DIR/snap1 $TEST_DIR/ploop2
ploop mount -r $TEST_DIR/ploop2/DiskDescriptor.xml
ploop snapshot $TEST_DIR/ploop2/DiskDescriptor.xml
ploop umount $TEST_DIR/ploop2/DiskDescriptor.xml
ploop mount $TEST_DIR/ploop2/DiskDescriptor.xml -m $TEST_DIR/ploop2-mnt

ls -l $TEST_DIR/ploop2-mnt
test -f $TEST_DIR/ploop2-mnt/test-file

# umount both ploop-s
ploop umount $TEST_DIR/ploop2/DiskDescriptor.xml
ploop umount $TEST_DIR/ploop1/DiskDescriptor.xml

rm -rf $TEST_DIR
