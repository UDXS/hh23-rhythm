THING="$WIN/mtw/MSOEtest2/CAPSENSE_Buttons_and_Slider_1"

cp $THING/{LICENSE,Makefile,README.md} $THING/build/*.* src/cypress
cp -r $THING/{deps,images,source} src/cypress
