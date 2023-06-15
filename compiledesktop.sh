g++ source/**.cpp /home/ya/code/superpooplib/src/**.cpp -o clayducks.out -Wall -I /home/ya/raylib-master/src/ -I headers/ -I /home/ya/code/superpooplib/headers/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DPLATFORM_DESKTOP -std=c++20
if [ $? -eq 0 ]; then
    ./clayducks.out
else
    echo "COMPILATION FAILED"
fi