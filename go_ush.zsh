echo
echo "\x1B[36m--------------------------------------- BASIC\x1B[0m"

echo
echo "\x1B[36m----stdin\x1B[0m"

diff <(echo "ls -la" | /bin/bash) <(echo "ls -la" | ./ush)

echo
echo "\x1B[36m----errors\x1B[0m"

echo
echo + 'ucod: command not found'
echo "ucod" | ./ush

echo
echo "\x1B[36m----ls -la, "echo"\x1B[0m"

diff <(echo "/bin/echo 55" | /bin/bash) <(echo "/bin/echo 55" | ./ush)
diff <(echo "/bin/ls -la" | /bin/bash) <(echo "/bin/ls -la" | ./ush)

echo
echo "\x1B[36m----exit 13\x1B[0m"

diff    <(echo 'exit 13' | /bin/bash ; echo \$\?) \
        <(echo 'exit 13' | ./ush ; echo \$\?)

echo
echo "\x1B[36m----echo\x1B[0m"

diff    <(echo 'echo "Tratata     ta ta"' | /bin/bash) \
        <(echo 'echo "Tratata     ta ta"' | ./ush)

diff    <(echo 'echo Tratata     ta ta' | /bin/bash) \
        <(echo 'echo Tratata     ta ta' | ./ush)

diff    <(echo 'echo -n "\a"' | /bin/bash) \
        <(echo 'echo -n "\a"' | ./ush)

echo "\x1B[31mcheck flag -E\x1B[0m"

#diff <(echo 'echo "T \n m \t s \v work correctly."' | /bin/bash)
#    <(echo 'echo "T \n m \t s \v work correctly."' | ./ush)

#diff <(echo 'echo -E "T \n m \t s \v work correctly."' | /bin/bash)
#    <(echo 'echo -E "T \n m \t s \v work correctly."' | ./ush)

diff    <(echo 'echo -e "\\windows"' | /bin/bash) \
        <(echo 'echo -e "\\windows"' | ./ush)

diff    <(echo 'echo -E "\\windows"' | /bin/bash) \
        <(echo 'echo -E "\\windows"' | ./ush)

echo
echo "\x1B[36m----"cd and pwd"\x1B[0m"

diff    <(echo 'cd / ; pwd' | /bin/bash) \
        <(echo 'cd / ; pwd' | ./ush)

cd /
cd -
echo
echo + поточна директорія
echo 'cd -; pwd' | ./ush
echo

diff    <(echo 'cd inc ; pwd' | /bin/bash) \
        <(echo 'cd inc ; pwd' | ./ush)

diff    <(echo 'cd; pwd' | /bin/bash) \
        <(echo 'cd; pwd' | ./ush)

diff    <(echo 'cd ~/Desktop; /bin/pwd' | /bin/bash) \
        <(echo 'cd ~/Desktop; /bin/pwd' | ./ush)

echo + '"/tmp: Not a directory"' і поточна директорія
echo 'cd -s /tmp; pwd' | ./ush

diff    <(echo 'cd /tmp; pwd -P' | /bin/bash) \
        <(echo 'cd /tmp; pwd -P' | ./ush)

diff    <(echo 'cd -P /tmp; pwd' | /bin/bash) \
        <(echo 'cd -P /tmp; pwd' | ./ush)

diff    <(echo 'cd /var; pwd -L; pwd -P' | /bin/bash) \
        <(echo 'cd /var; pwd -L; pwd -P' | ./ush)

echo
echo "\x1B[36m----"Escape characters"\x1B[0m"
echo + '"$(\)`file name' і '"dir `name"'
echo 'mkdir dir\ \`\\name;' | ./ush
echo 'cd dir\ \`\\name; touch \"\$\(\\\)\`file\ name; ls; pwd' | ./ush
echo 'rm -rf  dir\ \`\\name' | ./ush

echo
echo "\x1B[36m----"env"\x1B[0m" ---- "\x1B[31mручна перевірка\x1B[0m"
echo 'env -i emacs; echo $?' | ./ush
echo 'env -u TERM emacs; echo $?' | ./ush
echo 'env -P / date' | ./ush

echo
echo "\x1B[36m----"which"\x1B[0m"

echo 'which -s something; echo $?' | ./ush
echo 'which -s env; echo $?' | ./ush
echo 'which -a pwd' | ./ush

echo
echo "\x1B[36m----"export, unset"\x1B[0m"
echo 'export ucode=cbl; env' | ./ush
echo
echo 'unset ucode; env' | ./ush

echo
echo "\x1B[36m----"fg"\x1B[0m" ---- "\x1B[31mручна перевірка\x1B[0m"

echo
echo "\x1B[36m----"PATH variable"\x1B[0m"
diff    <(echo 'date; unset PATH; date' | /bin/zsh) \
        <(echo 'date; unset PATH; date' | ./ush)

echo
echo "\x1B[36m----"CTRL+C, CTRL+D"\x1B[0m" ---- "\x1B[31mручна перевірка\x1B[0m"

echo
echo "\x1B[36m----"Command separator ';'"\x1B[0m"
diff    <(echo 'echo 11 ; pwd ; echo 22 ; ls ; echo 33 ; ls -la' | /bin/zsh) \
        <(echo 'echo 11 ; pwd ; echo 22 ; ls ; echo 33 ; ls -la' | ./ush)
diff    <(echo ';' | /bin/zsh) \
        <(echo ';' | ./ush)
diff    <(echo ' ; ; ; ;' | /bin/zsh) \
        <(echo ' ; ; ; ;' | ./ush)

echo
echo "\x1B[36m----"Expansions';'"\x1B[0m"

diff    <(echo 'ls ~' | /bin/bash) \
        <(echo 'ls ~' | ./ush)

diff    <(echo 'ls ~/Desktop' | /bin/bash) \
        <(echo 'ls ~/Desktop' | ./ush)

diff    <(echo 'ls ~m' | /bin/bash) \
        <(echo 'ls ~m' | ./ush)

diff    <(echo 'echo `whoami`' | /bin/bash) \
        <(echo 'echo `whoami`' | ./ush)

diff    <(echo 'echo "$(echo -n "Ave"), $(echo -n "mo")!"' | /bin/bash) \
        <(echo 'echo "$(echo -n "Ave"), $(echo -n "mo")!"' | ./ush)

diff    <(echo 'echo "The user ${USER} is on a ${SHLVL} shell"' | /bin/bash) \
        <(echo 'echo "The user ${USER} is on a ${SHLVL} shell"' | ./ush)

diff    <(echo 'echo $PATH' | /bin/bash) \
        <(echo 'echo $PATH' | ./ush)

echo
echo "\x1B[36m----"Expansions hard"\x1B[0m"

diff    <(echo 'unset HOME; ls ~' | /bin/bash) \
        <(echo 'unset HOME; ls ~' | ./ush)

diff    <(echo 'unset USER; echo $USER' | /bin/bash) \
        <(echo 'unset USER; echo $USER' | ./ush)

diff    <(echo 'export d1="Hello," d2="World!"; echo "\$f1 = $d1, \$f2 = $d2"; unset d1 d2; echo "\$f1 = $d1, \$f2 = $d2"' | /bin/bash) \
        <(echo 'export d1="Hello," d2="World!"; echo "\$f1 = $d1, \$f2 = $d2"; unset d1 d2; echo "\$f1 = $d1, \$f2 = $d2"' | ./ush)

echo
echo "\x1B[36m----"extra check"\x1B[0m"
echo 'env -i ./ush
env -i env
ls -R / with signals, in comm subst
'
