'use client';
import Image from "next/image";
import note from "./note";

export default function Home() {
  return (
  <div>
    <header className="flex justify-center">
      <p>header</p>
    </header>
    <main className="flex justify-center">
      <section className="w-3/4">
        {note()}
      </section>
    </main>
    <footer className="flex justify-center">
      <p>footer</p>
    </footer>
  </div>
  );
}
