THING="$WIN/mtw/MSOEtest2/CAPSENSE_Buttons_and_Slider_1"

rm src/cypress/src/* src/cypress/deps/*
cp $THING/{LICENSE,Makefile,README.md} src/cypress
cp -r $THING/{deps,src} src/cypress
