/*
Apellido(s), nombre(s): Cardozo, Gonzalo Daniel
DNI: 43777470
Entrega: Sí

Apellido(s), nombre(s): Bruno, Yanil
DNI: 36992182
Entrega: Sí

Apellido(s), nombre(s): De Rogatis, Ramiro Javier
DNI: 44005090
Entrega: Sí

Apellido(s), nombre(s): Sgro, Nicolás Agustín
DNI: 44669267
Entrega: Sí
*/

#include "app.h"

int main(int argc, char *argv[]){
    App app;

    if (!appInit(&app)){
        return 1;
    }

    while (app.running){
        appHandleEvents(&app);
        appUpdate(&app);
        appRender(&app);
    }

    appCleanup(&app);

    return 0;
}
