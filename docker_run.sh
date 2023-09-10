docker build . -f Dockerfile -t rinha-c-interpreter

docker run \
    --rm \
    -v $(pwd)/sample.json:/var/rinha/source.rinha.json \
    rinha-c-interpreter
