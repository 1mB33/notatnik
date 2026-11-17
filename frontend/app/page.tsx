'use client';

import Note from "./note";
import React from "react";

type Status = "loading" | "ready" | "missing" | "error";
type NoteEntry = [name: string, id: number];

export default function Home() {
    const [notes, setNotes]                 = React.useState<NoteEntry[] | null>(null);
    const [selectedNote, setSelectedNote]   = React.useState(-1);
    const [listStatus, setListStatus]       = React.useState<Status>("loading");
    const [creating, setCreating]           = React.useState(false);
    const [createError, setCreateError]     = React.useState(false);
    const [query, setQuery]                 = React.useState("");
    const abortRef                          = React.useRef<AbortController | null>(null);

    const loadNotes = React.useCallback(async (signal?: AbortSignal) => {
        try {
            const response = await fetch("/api/list", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ ownerId: 1 }),
                signal,
            });

            if (response.status === 404) {
                return setListStatus("missing");
            }
            if (!response.ok) {
                return setListStatus("error");
            }

            const { notes } = await response.json();
            setNotes(Object.entries(notes) as NoteEntry[]);
            setListStatus("ready");
        } catch (err) {
            setListStatus("error");
        }
    }, []);

    React.useEffect(() => {
        const controller = new AbortController();
        loadNotes(controller.signal);
        return () => controller.abort();
    }, [loadNotes]);

    React.useEffect(() => () => abortRef.current?.abort(), []);

    async function handleCreate(e: React.FormEvent) {
        e.preventDefault();
        const noteName = query.trim();
        if (!noteName) {
            return;
        }

        abortRef.current?.abort();
        const controller = new AbortController();
        abortRef.current = controller;

        setCreating(true);
        setCreateError(false);
        try {
            const response = await fetch("/api/create", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify({ ownerId: 1, noteName }),
                signal: controller.signal,
            });

            if (!response.ok) return setCreateError(true);

            setQuery("");
            await loadNotes(controller.signal);
        } catch (err) {
            if ((err as Error).name === "AbortError") return;
            setCreateError(true);
        } finally {
            setCreating(false);
        }
    }

    return (
        <div className="w-3/4 flex m-w">
            <div className="h-full w-1/4">
                {!notes && listStatus}
                {notes?.map(([name, id]) => (
                    <div key={id} className="w-full px-5">
                        <button
                            type="button"
                            className="block w-full py-1 hover:bg-sky-700 active:bg-amber-950 overflow-x-hidden text-left"
                            onClick={() => setSelectedNote(id)}
                        >
                            {name}
                        </button>
                    </div>
                ))}
                <form className="flex justify-center w-full px-5 py-2" onSubmit={handleCreate}>
                    <input
                        name="query"
                        value={query}
                        onChange={(e) => setQuery(e.target.value)}
                        className="inline-block w-7/8 border-solid border-b-cyan-100 border-1"
                    />
                    <button
                        type="submit"
                        disabled={creating || !query.trim()}
                        className="inline-block p-3 py-1 hover:bg-sky-900 active:bg-amber-950 disabled:opacity-50"
                    >
                        +
                    </button>
                    {createError && <p className="text-red-500">Couldn't create note.</p>}
                </form>
            </div>
            <Note noteId={selectedNote} />
        </div>
    );
}
