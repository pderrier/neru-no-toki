@echo off
unalias *
del *.exe
alias /R c:\4dos\aliases

echo.
echo                €ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ€
echo                €       Compilationnage !        €
echo                €  (Femmes enceintes s'abstenir) €
echo                €‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹€
echo.

wpp386 nnt.cpp -i=e:\libs\ptc060\src -bt=dos -oneatx
IF NOT ERRORLEVEL=0 goto errortasm

echo.
echo ### COMPILATION OK
echo                  €ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ€
echo                  €  Linkation de la bàte !!  €
echo                  €    (Chaud devant...)      €
echo                  €‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹€

wlink @make.lnk
IF NOT ERRORLEVEL=0 goto errorlink
echo.
echo ### LINKAGE OK
goto ok

:errortasm
echo.
echo       ‹€€€€€€€€€€€€€€€€ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ€€€€€€€€€€€€€€€€€‹
echo      €€€€€€€€€€€€€€€€€  META-PROUT DE COMPILATION €€€€€€€€€€€€€€€€€€
echo       ﬂ€€€€€€€€€€€€€€€€‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹€€€€€€€€€€€€€€€€€ﬂ
goto fin

:errorlink
echo.
echo       ‹€€€€€€€€€€€€€€€€€€ﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂﬂ€€€€€€€€€€€€€€€€€€€‹
echo      €€€€€€€€€€€€€€€€€€€    ERREUR DE LINKAGE   €€€€€€€€€€€€€€€€€€€€
echo       ﬂ€€€€€€€€€€€€€€€€€€‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹‹€€€€€€€€€€€€€€€€€€€ﬂ
goto fin


:ok
echo.
echo Et zou !

univbe
nnt
univbe/U

:fin


