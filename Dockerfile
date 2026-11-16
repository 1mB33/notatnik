FROM ubuntu:latest AS build_dep
RUN apt-get update && apt-get install -y build-essential libpq-dev cmake git
ADD backend/deps/cpp-httplib /build/cpp-httplib
ADD backend/deps/json /build/json
ADD backend/deps/sqlpp11 /build/sqlpp11
WORKDIR /build/sqlpp11
RUN cmake -B build -DBUILD_POSTGRESQL_CONNECTOR=ON -DDEPENDENCY_CHECK=ON -DBUILD_TESTING=OFF && cmake --build build --target install

FROM build_dep AS build_app
ADD backend/app /build/app
ADD backend/database_manager /build/database_manager
ADD backend/Makefile /build/Makefile
WORKDIR /build
RUN make

FROM ubuntu:24.04 AS run_app
RUN apt-get update && apt-get install -y --no-install-recommends libpq5
COPY --from=build_app /build/bin/application /backend/app
COPY --from=build_app /build/lib /backend/lib
EXPOSE 8000
WORKDIR /backend
CMD [ "./app" ]

FROM node:22 AS base

FROM base AS frontend-base
ADD frontend/package.json /frontend/package.json
ADD frontend/package-lock.json /frontend/package-lock.json
ADD frontend/tsconfig.json  /frontend/tsconfig.json
ADD frontend/next.config.ts  /frontend/next.config.ts
ADD frontend/eslint.config.mjs  /frontend/eslint.config.mjs
ADD frontend/public  /frontend/public
ADD frontend/app  /frontend/app
WORKDIR /frontend
RUN npm install
RUN npm run build

FROM frontend-base AS frontend-run
WORKDIR /frontend
CMD ["npm", "run", "start"]
