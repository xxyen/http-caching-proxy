FROM gcc:9

RUN mkdir /var/log/erss
RUN mkdir /src
WORKDIR /src
ADD . /src/
ENTRYPOINT ["./main"]