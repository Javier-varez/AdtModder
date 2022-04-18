#!/bin/bash -e

clang_format_check() {
    echo "Checking format"
    find include src -type f \( -name '*.c' -or -name '*.h' -or -name '*.cpp' \) | xargs clang-format -Werror --dry-run
}

clang_format_run() {
    echo "Formatting"
    find include src -type f \( -name '*.c' -or -name '*.h' -or -name '*.cpp' \) | xargs clang-format -i
}

print_usage() {
    echo -e "$0 Available args:"
    echo -e "\tcheck - Checks the formatting of all source files in the src and includes folders"
    exit 1
}

if [ $# -eq 0 ]; then
    print_usage
fi

for arg in "$@"
do
    case "$arg" in
    check)
        clang_format_check
        ;;
    run)
        clang_format_run
        ;;
    *)
        print_usage
        ;;
    esac
done
