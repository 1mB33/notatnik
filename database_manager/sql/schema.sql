--
-- PostgreSQL database dump
--

\restrict orzNgGBb74M7sHtR6C2BnJfJ8sQAqIyr2IF6SuoMflG1cK4itfgbydiCdIcCaCJ

-- Dumped from database version 18.6
-- Dumped by pg_dump version 18.6

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: note; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE public.note (
    note_id integer NOT NULL,
    owner_id integer,
    local_path character varying(255)
);


--
-- Name: note note_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY public.note
    ADD CONSTRAINT note_pkey PRIMARY KEY (note_id);


--
-- PostgreSQL database dump complete
--

\unrestrict orzNgGBb74M7sHtR6C2BnJfJ8sQAqIyr2IF6SuoMflG1cK4itfgbydiCdIcCaCJ

