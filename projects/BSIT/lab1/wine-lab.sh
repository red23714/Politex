#!/bin/bash
set -e
EXE_DIR=${EXE_DIR:-$(dirname "$(readlink -f "$0")")}
BASE=${BASE:-$HOME/winelab}
BR=labbr
PORT=${PORT:-9000}
declare -A IP=( [client]=10.10.0.10 [srv1]=10.10.0.11 [srv2]=10.10.0.12 )
WINE=${WINE:-$(command -v wine64 || command -v wine || echo /usr/lib/wine/wine64)}

if [ "$(id -u)" -eq 0 ]; then SUDO=""; AS=""; else SUDO="sudo"; AS="sudo -u $(id -un)"; fi

# выполнить команду "на машине" $1
run_in() {
    local ns=$1; shift
    mkdir -p "$BASE/$ns"
    $SUDO ip netns exec "$ns" $AS env -u DISPLAY WINEDEBUG=-all WINEARCH=win64 \
        WINEPREFIX="$BASE/$ns" "$@"
}

case "$1" in
up)
    $SUDO ip link add $BR type bridge
    $SUDO ip addr add 10.10.0.1/24 dev $BR
    $SUDO ip link set $BR up
    for n in client srv1 srv2; do
        $SUDO ip netns add $n
        $SUDO ip link add v-$n type veth peer name e-$n
        $SUDO ip link set e-$n netns $n
        $SUDO ip link set v-$n master $BR up
        $SUDO ip netns exec $n ip addr add "${IP[$n]}/24" dev e-$n
        $SUDO ip netns exec $n ip link set e-$n up
        $SUDO ip netns exec $n ip link set lo up
    done
    echo "Сеть создана: client ${IP[client]}, srv1 ${IP[srv1]}, srv2 ${IP[srv2]} (мост $BR)"
    echo "Адрес сервера в клиенте: ${IP[srv1]} или ${IP[srv2]}"
    ;;
down)
    for n in client srv1 srv2; do $SUDO ip netns del $n 2>/dev/null || true; done
    $SUDO ip link del $BR 2>/dev/null || true
    echo "Сеть удалена"
    ;;
server1) run_in srv1 "$WINE" "$EXE_DIR/server.exe" "$PORT" ;;
server2) run_in srv2 "$WINE" "$EXE_DIR/server.exe" "$PORT" ;;
client)  run_in client "$WINE" "$EXE_DIR/client.exe" ;;
wireshark) $SUDO wireshark -i $BR -k -f "tcp port $PORT" & ;;
sniff)   $SUDO tcpdump -i $BR -nn -X "tcp port $PORT" ;;
flash)
    # flash srv1 on|off - "флешка" как папка $BASE/flash-<машина>, подключённая как диск E:
    m=$2; d="$BASE/$m/dosdevices"; f="$BASE/flash-$m"
    if [ "$3" = on ]; then
        mkdir -p "$f"; ln -sfn "$f" "$d/e:"; echo "E: подключён ($f). Перезапустите сервер на $m."
    else
        rm -f "$d/e:"; echo "E: отключён. Перезапустите сервер на $m."
    fi
    ;;
*)
    sed -n '2,19p' "$0"; exit 1 ;;
esac
