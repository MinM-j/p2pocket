#!/bin/bash

tmux kill-pane -a -t 0

tmux send-keys "cls" C-m
tmux send-keys "rm -r p2pocket_data" C-m
tmux send-keys "./Bootstrap" C-m

#tmux send-keys "sleep 1 && cd sth1" C-m
#tmux send-keys "clear && p2p_cli 1111 127.0.0.1:8888" C-m

tmux split-window -h 
tmux send-keys "cls && sleep 1 && ./Minor peer1 1111" C-m

tmux split-window -v
tmux send-keys "cls && sleep 2 && ./Minor peer2 2222" C-m

tmux select-pane -t 0

tmux split-window -v
tmux send-keys "cls && sleep 3 && ./Minor peer3 3333" C-m

tmux select-pane -t 1

#===========================================#
#tmux new-window 
#tmux send-keys "cls && sleep 1 && ./Minor peer4 4444" C-m

#tmux split-window -h 
#tmux send-keys "cls && sleep 2 && ./Minor peer5 5555" C-m
#
#tmux split-window -v
#tmux send-keys "cls && sleep 3 && ./Minor peer6 6666" C-m
#
#tmux select-pane -t 0
#
#tmux split-window -v
#tmux send-keys "cls && sleep 3 && ./Minor peer7 7777" C-m
#
#tmux select-pane -t 0
