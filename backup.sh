#!/bin/bash
set -e

backup_from="${backup_from:-"${XDG_CONFIG_HOME:-~/.config}/banner_launcher"}"
backup_from_target="$(basename "$backup_from")"
backup_from_real_dir="$(dirname "$(realpath --canonicalize-missing "$backup_from")")"

backup_to="${backup_to:-"backup/$(date -u +"%Y_%m_%d_%H_%M_%S").tar.gz"}"
backup_to_dir="$(dirname "$backup_to")"
backup_to_real_path="$(realpath --canonicalize-missing "$backup_to")"

echo "Backing up $backup_from to $backup_to"

mkdir -p "$backup_to_dir"

pushd "$backup_from_real_dir" > /dev/null
tar czf "${backup_to_real_path}" --absolute-names "${backup_from_target}"
popd > /dev/null
