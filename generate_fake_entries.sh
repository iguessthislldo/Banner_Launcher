# Generate Fake Entries with fake images
d=debug_config
n=50 # Number of Entries to Fake

rm -fr $d
./banner_launcher -d --genconf

function make_entry {
cat << EOF >> $d/entries.ini

[$1]
name=Entry $1
image=$1.png
count=0
favorite=false
exec=true"
EOF
    convert \
        -size 460x215 \
        -font fixed -pointsize 56 \
        $(python color.py) \
        -gravity center \
        "label:Entry $1" \
        debug_config/banners/$1.png
}

echo Creating Fake Entries...
cat << EOF > $d/entries.ini
[meta]
next_id=$(expr $n + 1)
EOF
for i in $(seq --equal-width 1 $n)
do
    echo "  #$i"
    make_entry $i
done
echo Done!
