FROM ubuntu:latest AS build_dep
RUN apt-get update && apt-get install -y build-essential libpq-dev cmake git
ADD cpp-httplib /build/cpp-httplib
ADD json /build/json
ADD sqlpp11 /build/sqlpp11
WORKDIR /build/sqlpp11
RUN cmake -B build -DBUILD_POSTGRESQL_CONNECTOR=ON -DDEPENDENCY_CHECK=ON -DBUILD_TESTING=OFF && cmake --build build --target install

FROM build_dep AS build_app
ADD app /build/app
ADD database_manager /build/database_manager
ADD Makefile /build/Makefile
WORKDIR /build
RUN make

FROM ubuntu:24.04 AS run_app
RUN apt-get update && apt-get install -y --no-install-recommends libpq5
COPY --from=build_app /build/bin/application /app
COPY --from=build_app /build/lib /lib
EXPOSE 8000
CMD [ "/app" ]
