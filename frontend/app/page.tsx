'use client';
import Image from "next/image";
import note from "./note";

export default function Home() {
    return (
        <section className="w-3/4">
            {note()}
        </section>
    );
}
