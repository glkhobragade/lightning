#include <lightningd/key_derive.h>
#include <lightningd/keyset.h>

bool derive_keyset(const struct pubkey *per_commitment_point,
		   const struct pubkey *self_payment_basepoint,
		   const struct pubkey *other_payment_basepoint,
		   const struct pubkey *self_delayed_basepoint,
		   const struct pubkey *other_revocation_basepoint,
		   struct keyset *keyset)
{
	/* BOLT #3:
	 *
	 * ### `localkey`, `remotekey`, `local_delayedkey` and
         *     `remote_delayedkey` Derivation
	 *
	 * These keys are simply generated by addition from their base points:
	 *
	 *	pubkey = basepoint + SHA256(per_commitment_point || basepoint)*G
	 *
	 * The `localkey` uses the local node's `payment_basepoint`,
	 * `remotekey` uses the remote node's `payment_basepoint`, the
	 * `local_delayedkey` uses the local node's
	 * `delayed_payment_basepoint`, and the `remote_delayedkey` uses the
	 * remote node's `delayed_payment_basepoint`.
	 */
	if (!derive_simple_key(self_payment_basepoint,
			       per_commitment_point,
			       &keyset->self_payment_key))
		return false;

	if (!derive_simple_key(other_payment_basepoint,
			       per_commitment_point,
			       &keyset->other_payment_key))
		return false;

	if (!derive_simple_key(self_delayed_basepoint,
			       per_commitment_point,
			       &keyset->self_delayed_payment_key))
		return NULL;

	/* BOLT #3:
	 *
	 * ### `revocationkey` Derivation
	 *
	 * The `revocationkey` is a blinded key: when a node wishes to create
	 * a new commitment for a remote node, it uses its own
	 * `revocation_basepoint` and the remote node's `per_commitment_point`
	 * to derive a new `revocationkey` for the commitment.
	 */
	if (!derive_revocation_key(other_revocation_basepoint,
				   per_commitment_point,
				   &keyset->self_revocation_key))
		return false;

	return true;
}
