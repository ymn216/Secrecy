# mkdir build
# cd build
# cmake ..
# make -j1

cd build
make planner
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <query_file> <schema_file> <param1> [additional_params...]"
    exit 1
fi

QUERY_FILE="../examples/queries/$1"
SCHEMA_FILE="../examples/schemas/$2"
PARAM1=$3
shift 3

mpirun --allow-run-as-root -np 3 ./planner "$QUERY_FILE" "$SCHEMA_FILE" "$PARAM1" "$@"

