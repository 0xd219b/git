#include "git-compat-util.h"
#include "test-tool.h"
#include "strbuf.h"
#include "object-store.h"
#include "packfile.h"
#include "pack-mtimes.h"

static int dump_mtimes(struct packed_git *p)
{
	uint32_t i;
	if (load_pack_mtimes(p) < 0)
		die("could not load pack .mtimes");

	for (i = 0; i < p->num_objects; i++) {
		struct object_id oid;
		if (nth_packed_object_id(&oid, p, i) < 0)
			die("could not load object id at position %"PRIu32, i);

		printf("%s %"PRIu32"\n",
		       oid_to_hex(&oid), nth_packed_mtime(p, i));
	}

	return 0;
}

static const char *pack_mtimes_usage = "\n"
"  test-tool pack-mtimes <pack-name.mtimes>";

int cmd__pack_mtimes(int argc, const char **argv)
{
	struct strbuf buf = STRBUF_INIT;
	struct packed_git *p;

	setup_git_directory();

	if (argc != 2)
		usage(pack_mtimes_usage);

	for (p = get_all_packs(the_repository); p; p = p->next) {
		strbuf_addstr(&buf, basename(p->pack_name));
		strbuf_strip_suffix(&buf, ".pack");
		strbuf_addstr(&buf, ".mtimes");

		if (!strcmp(buf.buf, argv[1]))
			break;

		strbuf_reset(&buf);
	}

	strbuf_release(&buf);

	return p ? dump_mtimes(p) : 1;
}
