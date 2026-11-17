"use client";

import React from "react";

type Status = "loading" | "ready" | "missing" | "error";

export interface INote {
    noteId: number,
}

export default function Note({ noteId }: INote) {
    const [content, setContent]     = React.useState("");
    const [status, setStatus]       = React.useState<Status>("loading");
    const textareaRef               = React.useRef<HTMLTextAreaElement>(null);

    const fit = React.useCallback(() => {
        const el = textareaRef.current;
        if (!el) {
            return;
        }
        el.style.height = "auto";
        el.style.height = `${el.scrollHeight + (el.offsetHeight - el.clientHeight)}px`;
    }, []);

    React.useLayoutEffect(fit, [content, status, fit]);

    React.useEffect(() => {
        window.addEventListener("resize", fit);
        return () => window.removeEventListener("resize", fit);
    }, [fit]);

    React.useEffect(() => {
        const controller = new AbortController();

        (async () => {
            try {
                const response = await fetch("/api/read", {
                    method: "POST",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ ownerId: 1, noteId: noteId }),
                    signal: controller.signal,
                });

                if (response.status === 404) {
                    return setStatus("missing");
                }
                if (!response.ok) {
                    return setStatus("error");
                }

                const data = await response.json();
                setContent(data?.noteContent ?? "");
                setStatus("ready");
            } catch (err) {
                setStatus("error");
            }
        })();

        return () => controller.abort();
    }, [noteId]);

    function handleSave() {
        const controller = new AbortController();

        (async () => {
            try {
                const response = await fetch("/api/update", {
                    method: "PATCH",
                    headers: { "Content-Type": "application/json" },
                    body: JSON.stringify({ ownerId: 1, noteId: noteId, noteContent: content }),
                    signal: controller.signal,
                });

                if (response.ok) {
                    alert("saved")
                } else {
                    alert("error")
                }
            } catch (err) {
                setStatus("error");
            }
        })();

        return () => controller.abort();
    }

    if (noteId === -1) {
        return <span>not selected</span>;
    }
    if (status === "loading") {
        return <span>...</span>;
    }
    if (status === "missing") {
        return <span>missing</span>;
    }
    if (status === "error") {
        return <span>error</span>;
    }

    return (
        <span className="pt-10 pb-10 w-3/4 inline-block">
            <textarea
                ref={textareaRef}
                rows={1}
                value={content}
                onChange={(e) => setContent(e.target.value)}
                className="box-border w-full resize-none overflow-hidden border-none outline-none py-5 px-10 bg-cyan-950"
            />
            <button className="mt-5 p-1 hover:bg-sky-700 active:bg-amber-950" type="button" onClick={handleSave}>
                save
            </button>
        </span>
    );
}
