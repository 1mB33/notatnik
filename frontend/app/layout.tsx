import type { Metadata } from "next";
import { Geist, Geist_Mono } from "next/font/google";
import "./globals.css";

const geistSans = Geist({
    variable: "--font-geist-sans",
    subsets: ["latin"],
});

const geistMono = Geist_Mono({
    variable: "--font-geist-mono",
    subsets: ["latin"],
});

export const metadata: Metadata = {
    title: "Amazing notatnik",
    description: "Wow",
};

export default function RootLayout({ children }: LayoutProps<"/">) {
    return (
        <html
            lang="en"
            className={`${geistSans.variable} ${geistMono.variable} h-full antialiased`}
        >
            <body className="min-h-full flex flex-col">
                <header className="flex justify-center">
                    <p>header</p>
                </header>
                <main className="flex justify-center">
                    {children}
                </main>
                <footer className="flex justify-center">
                    <p>footer</p>
                </footer>
            </body>
        </html>
    );
}
